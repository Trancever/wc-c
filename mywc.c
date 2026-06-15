#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "utf_8.h"

#define BLOCK_SIZE 4096

struct file_counts {
  uint64_t bytes;
  uint64_t words;
  uint64_t lines;
  uint64_t characters;
  uint64_t max_line_length_bytes;
  uint64_t max_line_length_characters;
};

struct file_parse_result {
  struct file_counts counts;
  int read_error_number;
};

struct output_options {
  bool show_lines;
  bool show_words;
  bool show_bytes;
  bool show_max_line_length;
  bool show_characters;
  bool has_error;
};

struct file_parse_result parse_file(int fd) {
  struct file_parse_result parse_result = {0};
  ssize_t bytes_read = -1;
  unsigned char buffer[BLOCK_SIZE];
  bool in_word = false;
  uint64_t current_line_length_bytes = 0;
  uint64_t current_line_length_characters = 0;

  ssize_t bytes_to_skip = 0;

  while ((bytes_read = read(fd, buffer, BLOCK_SIZE)) > 0) {
    parse_result.counts.bytes += (uint64_t)bytes_read;

    ssize_t i = bytes_to_skip;
    while (i < bytes_read) {
      struct decode_result decoded = decode_leading_byte(buffer[i]);
      parse_result.counts.characters++;

      unsigned char ch = buffer[i];

      if (ch == '\n') {
        parse_result.counts.lines++;

        parse_result.counts.max_line_length_bytes =
            current_line_length_bytes >
                    parse_result.counts.max_line_length_bytes
                ? current_line_length_bytes
                : parse_result.counts.max_line_length_bytes;
        current_line_length_bytes = 0;

        parse_result.counts.max_line_length_characters =
            current_line_length_characters >
                    parse_result.counts.max_line_length_characters
                ? current_line_length_characters
                : parse_result.counts.max_line_length_characters;
        current_line_length_characters = 0;
      } else {
        current_line_length_characters++;
        current_line_length_bytes += decoded.num_of_bytes;
      }

      int is_space = isspace(ch);

      if (!is_space && in_word == false) {
        parse_result.counts.words++;
      }

      in_word = is_space ? 0 : 1;
      i += decoded.num_of_bytes;
    }

    bytes_to_skip = i - bytes_read;
  }

  if (bytes_read == -1) {
    parse_result.read_error_number = errno;
  } else {
    if (current_line_length_bytes > parse_result.counts.max_line_length_bytes) {
      parse_result.counts.max_line_length_bytes = current_line_length_bytes;
    }

    if (current_line_length_characters >
        parse_result.counts.max_line_length_characters) {
      parse_result.counts.max_line_length_characters =
          current_line_length_characters;
    }
  }

  return parse_result;
}

void add_counts(struct file_counts *total_counts,
                const struct file_counts *counts_to_add) {
  total_counts->bytes += counts_to_add->bytes;
  total_counts->lines += counts_to_add->lines;
  total_counts->words += counts_to_add->words;
  total_counts->characters += counts_to_add->characters;
}

void print_counts_line(const struct file_counts *counts, const char *name,
                       const struct output_options *options) {
  if (options->show_lines) {
    printf("%8" PRIu64, counts->lines);
  }

  if (options->show_words) {
    printf("%8" PRIu64, counts->words);
  }

  if (options->show_bytes) {
    printf("%8" PRIu64, counts->bytes);
  }

  if (options->show_characters) {
    printf("%8" PRIu64, counts->characters);
  }

  if (options->show_max_line_length) {
    uint64_t value_to_show = options->show_characters
                                 ? counts->max_line_length_characters
                                 : counts->max_line_length_bytes;

    printf("%8" PRIu64, value_to_show);
  }

  if (name != NULL) {
    printf(" %s", name);
  }

  printf("\n");
}

void print_read_error(int error_number, const char *name) {
  fprintf(stderr, "mywc: %s: read: %s\n", name, strerror(error_number));
}

struct output_options parse_command_line_args(int argc, char *argv[]) {
  struct output_options options = {0};

  int opt = 0;

  while ((opt = getopt(argc, argv, "lwcLm")) != -1) {
    switch (opt) {
    case 'l':
      options.show_lines = true;
      break;
    case 'w':
      options.show_words = true;
      break;
    case 'c':
      options.show_bytes = true;
      break;
    case 'L':
      options.show_max_line_length = true;
      break;
    case 'm':
      options.show_characters = true;
      break;
    default:
      options.has_error = true;
      break;
    }
  }

  if (!options.show_lines && !options.show_bytes && !options.show_words &&
      !options.show_max_line_length && !options.show_characters) {
    options.show_lines = true;
    options.show_bytes = true;
    options.show_words = true;
  }

  return options;
}

int main(int argc, char *argv[]) {
  struct output_options options = parse_command_line_args(argc, argv);

  if (options.has_error) {
    return 1;
  }

  if (optind == argc) {
    struct file_parse_result parse_result = parse_file(STDIN_FILENO);

    if (parse_result.read_error_number) {
      print_read_error(parse_result.read_error_number, "standard input");
      return 1;
    }

    print_counts_line(&parse_result.counts, NULL, &options);
    return 0;
  }

  struct file_counts total_counts = {0};

  int file_index = optind;
  const int files_to_process = argc - optind;

  int files_processed_successfully = 0;

  while (file_index < argc) {
    const char *file_name = argv[file_index];

    bool is_stdin = strcmp(file_name, "-") == 0;
    int fd = -1;

    if (is_stdin) {
      fd = STDIN_FILENO;
    } else {
      fd = open(file_name, O_RDONLY);
    }

    if (fd == -1) {
      int error_number = errno;
      fprintf(stderr, "mywc: %s: open: %s\n", file_name,
              strerror(error_number));

      file_index++;
      continue;
    }

    struct file_parse_result parse_result = parse_file(fd);

    if (parse_result.read_error_number) {
      print_read_error(parse_result.read_error_number, file_name);
    } else {
      print_counts_line(&parse_result.counts, file_name, &options);
      add_counts(&total_counts, &parse_result.counts);
      files_processed_successfully++;

      if (parse_result.counts.max_line_length_bytes > total_counts.max_line_length_bytes) {
        total_counts.max_line_length_bytes = parse_result.counts.max_line_length_bytes;
      }

      if (parse_result.counts.max_line_length_characters > total_counts.max_line_length_characters) {
        total_counts.max_line_length_characters = parse_result.counts.max_line_length_characters;
      }
    }

    if (!is_stdin) {
      close(fd);
    }

    file_index++;
  }

  if (files_processed_successfully > 1) {
    print_counts_line(&total_counts, "total", &options);
  }

  if (files_processed_successfully < files_to_process) {
    return 1;
  }

  return 0;
}
