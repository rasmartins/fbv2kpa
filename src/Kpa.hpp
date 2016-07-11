#ifndef KPA_HPP_INCLUDED_
#define KPA_HPP_INCLUDED_

#include <stdint.h>

#include "Note.hpp"

#define FBV_NOTE_PACK(note, sharp) (note | sharp << 7)
#define FBV_NOTE_NAME(note_pak)    (note_pak & 0x7f)
#define FBV_NOTE_SHARP(note_pak)   ((note_pak >> 7) != 0)

const static uint8_t c_note_table[] =
{
  FBV_NOTE_PACK('C', 0),
  FBV_NOTE_PACK('C', 1),
  FBV_NOTE_PACK('D', 0),
  FBV_NOTE_PACK('D', 1),
  FBV_NOTE_PACK('E', 0),
  FBV_NOTE_PACK('F', 0),
  FBV_NOTE_PACK('F', 1),
  FBV_NOTE_PACK('G', 0),
  FBV_NOTE_PACK('G', 1),
  FBV_NOTE_PACK('A', 0),
  FBV_NOTE_PACK('A', 1),
  FBV_NOTE_PACK('B', 0),
  FBV_NOTE_PACK(' ', 0)
};


class Kpa
{
public:
  Kpa(Serial& uart):
    m_uart(uart),
    m_state(KPA_STATE_ASENSE)
  { }

  void
  update(void)
  {
    uint8_t byte = 0;
    if (!m_uart.read(&byte))
      return;

    //std::fprintf(stderr, " %02X", byte);

    switch (m_state)
    {
      case KPA_STATE_ASENSE:
        if (byte == 0xFE)
        {
          sendBeacon();
          sendOwner();
        }
        else if (byte == 0xF0)
        {
          m_syx_idx = 0;
          m_state = KPA_STATE_SYX;
        }
        else
        {
          std::fprintf(stderr, "discard: %02X\r\n", byte);
        }
        break;

      case KPA_STATE_SYX:
        if (byte == 0xF7)
        {
          handleSysEx(m_syx, m_syx_idx);
          m_state = KPA_STATE_ASENSE;
        }
        else
        {
          m_syx[m_syx_idx++] = byte;
        }
        break;
    }
  }

private:
  enum State
  {
    KPA_STATE_ASENSE,
    KPA_STATE_SYX,
  };

  Serial& m_uart;
  State m_state;
  uint8_t m_syx[128];
  size_t m_syx_idx;
  Note m_note;
  int m_tuning;

  void
  handleSysEx(const uint8_t* data, size_t size)
  {
    // std::fprintf(stderr, "SYX:");
    // for (size_t i = 0; i < size; ++i)
    //   std::fprintf(stderr, " %02X", data[i]);
    // std::fprintf(stderr, "\n");

    if (data[7] == 0x7D && data[8] == 0x54)
    {
      onCurrentNote(data, size);
    }
    else if (data[7] == 0x7C && data[8] == 0x0F)
    {
      onCurrentTuning(data, size);
      requestCurrentNote();
    }
  }

  void
  sendCommand(const uint8_t* cmd, size_t size)
  {
    for (size_t i = 0; i < size; ++i)
    {
      m_uart.write(cmd[i]);
    }
  }

  void
  sendBeacon(void)
  {
    uint8_t cmd[] = {0xF0, 0x00, 0x20, 0x33, 0x02, 0x7F, 0x7E, 0x00, 0x40, 0x01, 0x36, 0x04, 0xF7};
    sendCommand(cmd, sizeof(cmd));
  }

  void
  sendOwner(void)
  {
    uint8_t cmd[] = {0xF0, 0x00, 0x20, 0x33, 0x02, 0x7f, 0x03, 0x7F, 0x7F, 'F', 'B', 'V', '2', 'K', 'P', 'A', 0xF7};
    sendCommand(cmd, sizeof(cmd));
  }

  void
  onCurrentTuning(const uint8_t* data, size_t size)
  {
    uint16_t value = (data[9] & 0x7F) << 7;

    if (size == 11)
      value |= (data[10] & 0x7F);

    m_tuning = value - (16383 / 2);
  }

  void
  onCurrentNote(const uint8_t* data, size_t size)
  {
    if (data[10] == 0)
    {
      m_note.setInvalid();
      return;
    }

    uint8_t octave = (data[10] / 12) - 1;
    uint8_t note_idx = data[10] % 12;
    uint8_t note_packed = c_note_table[note_idx];

    m_note.setName(FBV_NOTE_NAME(note_packed));
    m_note.setAccidental(FBV_NOTE_SHARP(note_packed) ? Note::ACC_SHARP : Note::ACC_NATURAL);
    m_note.setOctave(octave);

    m_note.dump();
  }

  void
  requestCurrentNote(void)
  {
    uint8_t cmd[] = {0xF0, 0x00, 0x20, 0x33, 0x02, 0x7f, 0x41, 0x00, 0x7D & 0x7F, 0xD4 & 0x7F, 0xF7};
    sendCommand(cmd, sizeof(cmd));
  }
};

#endif
