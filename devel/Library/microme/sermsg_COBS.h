/*
COBS: Consistent Overhead Byte Stuffing
  https://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing

COBS makes arrays of transport data free of the zero element value
so it can be used to delimit packets. Also it has a constant enlargement
to the data array size so it is predictable.

COBS in-place encoder:
This encodes an array of (up to) 128bytes to a COBS packet
It is important, that the array for storing the data needs to
be 4 bytes into the output array. (4bytes to make sure it alings to 32 bits)
*/

static uint8_t COBS_encode128(volatile uint8_t* buffer, uint8_t length)
{
  uint8_t read_pos  = 4;
  uint8_t write_pos = 1;
  uint8_t code_pos  = 0;
  uint8_t code      = 1;

  if (!length)
    return 0;

  while (read_pos < length+4) {
    if (buffer[read_pos] == 0) {
      buffer[code_pos] = code;
      code = 1;
      code_pos = write_pos++;
      read_pos++;
    }
    else {
      buffer[write_pos++] = buffer[read_pos++];
      code++;

      if (code == 0xFF) {
        buffer[code_pos] = code;
        code = 1;
        code_pos = write_pos++;
      }
    }
  }

  buffer[code_pos] = code;

  return write_pos;
}

/*
COBS in-place decoder:
This decodes an array of (up to) 128bytes from a COBS packet
*/
static uint8_t COBS_decode128(volatile uint8_t* buffer, uint8_t length)
{
  if (!length)
      return 0;

  uint8_t read_pos  = 0;
  uint8_t write_pos = 0;
  uint8_t code      = 0;
  uint8_t i         = 0;

  while (read_pos < length) {
    code = buffer[read_pos];

    if (read_pos + code > length && code != 1)
      return 0;

    read_pos++;

    for (i = 1; i < code; i++)
      buffer[write_pos++] = buffer[read_pos++];

    if (code != 0xFF && read_pos != length)
      buffer[write_pos++] = 0;
  }

  return write_pos;
}
