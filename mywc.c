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
#include "unicode.h"

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

struct file_parse_state {
  bool in_word;
  uint64_t current_line_length_bytes;
  uint64_t current_line_length_characters;
};

struct output_options {
  bool show_lines;
  bool show_words;
  bool show_bytes;
  bool show_max_line_length;
  bool show_characters;
  bool has_error;
};

uint64_t max_uint64_t(uint64_t a, uint64_t b) { return a > b ? a : b; }

void update_state_on_new_character(unsigned char character,
                                   struct utf8_decode_result decoded,
                                   struct file_parse_result *parse_result,
                                   struct file_parse_state *parse_state) {
  if (decoded.status == UTF8_VALID) {
    parse_result->counts.characters++;

    if (character == '\n') {
      parse_result->counts.lines++;

      parse_result->counts.max_line_length_bytes =
          max_uint64_t(parse_state->current_line_length_bytes,
                       parse_result->counts.max_line_length_bytes);
      parse_state->current_line_length_bytes = 0;

      parse_result->counts.max_line_length_characters =
          max_uint64_t(parse_state->current_line_length_characters,
                       parse_result->counts.max_line_length_characters);
      parse_state->current_line_length_characters = 0;
    } else {
      parse_state->current_line_length_characters++;
      parse_state->current_line_length_bytes += decoded.bytes_consumed;
    }

    bool is_white_space_character = is_unicode_whitespace(decoded.code_point);

    if (!is_white_space_character && parse_state->in_word == false) {
      parse_result->counts.words++;
    }

    parse_state->in_word = is_white_space_character ? 0 : 1;
  }
}

struct file_parse_result parse_file(int fd) {
  struct file_parse_result parse_result = {0};
  ssize_t bytes_read = -1;
  unsigned char buffer[BLOCK_SIZE];

  uint8_t carry_over_char_length = 0;
  struct file_parse_state file_parse_state = {0};

  while ((bytes_read = read(fd, buffer + carry_over_char_length,
                            BLOCK_SIZE - carry_over_char_length)) > 0) {
    parse_result.counts.bytes += (uint64_t)bytes_read;

    ssize_t i = 0;
    ssize_t available_bytes = bytes_read + carry_over_char_length;

    while (i < available_bytes) {
      struct utf8_decode_result decoded =
          decode_utf8_char(buffer + i, (uint64_t)(available_bytes - i));

      update_state_on_new_character(buffer[i], decoded, &parse_result,
                                    &file_parse_state);

      if (decoded.status == UTF8_INCOMPLETE) {
        uint8_t idx;

        // copy leading byte and all continuation bytes that are part of current
        // buffer to start of buffer
        for (idx = 0; idx < decoded.expected_bytes && idx + i < available_bytes;
             idx++) {
          buffer[idx] = buffer[i + idx];
        }

        carry_over_char_length = idx;
        break;
      } else {
        carry_over_char_length = 0;
        i += decoded.bytes_consumed;
      }
    }
  }

  if (bytes_read == -1) {
    parse_result.read_error_number = errno;
  } else {
    parse_result.counts.max_line_length_bytes =
        max_uint64_t(file_parse_state.current_line_length_bytes,
                     parse_result.counts.max_line_length_bytes);
    parse_result.counts.max_line_length_characters =
        max_uint64_t(file_parse_state.current_line_length_characters,
                     parse_result.counts.max_line_length_characters);
  }

  return parse_result;
}

void add_counts(struct file_counts *total_counts,
                const struct file_counts *counts_to_add) {
  total_counts->bytes += counts_to_add->bytes;
  total_counts->lines += counts_to_add->lines;
  total_counts->words += counts_to_add->words;
  total_counts->characters += counts_to_add->characters;

  total_counts->max_line_length_bytes =
      max_uint64_t(counts_to_add->max_line_length_bytes,
                   total_counts->max_line_length_bytes);
  total_counts->max_line_length_characters =
      max_uint64_t(counts_to_add->max_line_length_characters,
                   total_counts->max_line_length_characters);
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
