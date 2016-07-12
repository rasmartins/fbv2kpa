//***************************************************************************
// This file is part of fbv2kpa.                                            *
//***************************************************************************
// fbv2kpa is free software: you can redistribute it and/or modify          *
// it under the terms of the GNU General Public License as published by     *
// the Free Software Foundation, either version 3 of the License, or        *
// (at your option) any later version.                                      *
//                                                                          *
// fbv2kpa is distributed in the hope that it will be useful,               *
// but WITHOUT ANY WARRANTY; without even the implied warranty of           *
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
// GNU General Public License for more details.                             *
//                                                                          *
// You should have received a copy of the GNU General Public License        *
// along with Foobar.  If not, see <http://www.gnu.org/licenses/>.          *
//***************************************************************************

#ifndef KPA_HPP_INCLUDED_
#define KPA_HPP_INCLUDED_

// ISO C++ 11 headers.
#include <cstdint>

// Local headers.
#include "Note.hpp"

class Kpa
{
public:
  Kpa(Serial& uart):
    m_uart(uart),
    m_state(KPA_STATE_ASENSE),
    m_note(getNoteByIndex(c_note_invalid_idx))
  { }

  void
  update(void)
  {
    uint8_t byte = 0;
    if (!m_uart.read(&byte))
      return;

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

  //! Index of the invalid note.
  static const int c_note_invalid_idx = 12;

  //! UART handle.
  Serial& m_uart;
  State m_state;
  uint8_t m_syx[128];
  size_t m_syx_idx;
  //! Current note.
  Note& m_note;
  //! Current tune value.
  int m_tuning;

  Note&
  getNoteByIndex(int index)
  {
    static Note note_table[] =
    {
      Note('C', Note::ACC_NATURAL),
      Note('C', Note::ACC_SHARP),
      Note('D', Note::ACC_NATURAL),
      Note('D', Note::ACC_SHARP),
      Note('E', Note::ACC_NATURAL),
      Note('F', Note::ACC_NATURAL),
      Note('F', Note::ACC_SHARP),
      Note('G', Note::ACC_NATURAL),
      Note('G', Note::ACC_SHARP),
      Note('A', Note::ACC_NATURAL),
      Note('A', Note::ACC_SHARP),
      Note('B', Note::ACC_NATURAL),
      Note(' ', Note::ACC_NATURAL)
    };

    Note& note = note_table[index];
    return note;
  }

  void
  saveCurrentNote(int index, int octave)
  {
    m_note = getNoteByIndex(index);
    m_note.setOctave(octave);
  }

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
    requestCurrentNote();

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

    saveCurrentNote(note_idx, octave);
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
