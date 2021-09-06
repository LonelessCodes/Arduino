struct {
  unsigned short bpm;
  unsigned long beat_start = 0;
  unsigned long beat_offset = 0;
  unsigned long beat_len;

  void set(byte new_speed) {
    if (new_speed == bpm) return;

    bpm = new_speed;
    beat_len = 60000L / bpm;
  }

  float getCurrBeat(unsigned int num_beats) {
    unsigned long full_len = num_beats * beat_len;
    unsigned long time_ago = (millis() - beat_start) % full_len;

    return (float)time_ago / beat_len;
  }

  unsigned long timeToNextFrame(unsigned int beat, unsigned long offset) {
    unsigned long full_len = beat * beat_len;
    unsigned long time_ago = (millis() - beat_start - (offset * beat_len)) % full_len;

    return full_len - time_ago;
  }
} Speed;