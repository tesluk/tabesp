CRGB colorFromHex(char* in)
{
  if (in[0] == 0) return CRGB::White;
  char first = in[0];
  uint32_t c = 0;

  if (first == '#' || first == 'h' || first == 'H') //is HEX encoded
  {
    c = strtoul(in + 1, NULL, 16);
  } else
  {
    c = strtoul(in, NULL, 10);
  }

  return CRGB((c >> 16) & 0xFF, (c >> 8) & 0xFF, c & 0xFF );
}
