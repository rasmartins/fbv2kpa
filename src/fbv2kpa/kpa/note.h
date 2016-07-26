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
// along with fbv2kpa. If not, see <http://www.gnu.org/licenses/>.          *
//***************************************************************************

#ifndef KPA_NOTE_H_INCLUDED_
#define KPA_NOTE_H_INCLUDED_

// ISO C 99 headers.
#include <stdint.h>

enum kpa_note_accidental
{
  KPA_NA_NATURAL = 0x00,
  KPA_NA_FLAT    = 0x01,
  KPA_NA_SHARP   = 0x02
};

//! Invalid note name.
static const char c_kpa_note_invalid_name = '_';

typedef enum kpa_note_accidental kpa_note_accidental_t;

#define KPA_NOTE_PACK(name, accidental) ((name - 'A') << 2 | accidental)
#define KPA_NOTE_NAME(packed)           ((packed >> 2) + 'A')
#define KPA_NOTE_ACCD(packed)           (packed & 0x03)

//! Kemper tuner note table.
static const uint8_t c_kpa_note_table[] =
{
  KPA_NOTE_PACK('C', KPA_NA_NATURAL),
  KPA_NOTE_PACK('C', KPA_NA_SHARP),
  KPA_NOTE_PACK('D', KPA_NA_NATURAL),
  KPA_NOTE_PACK('D', KPA_NA_SHARP),
  KPA_NOTE_PACK('E', KPA_NA_NATURAL),
  KPA_NOTE_PACK('F', KPA_NA_NATURAL),
  KPA_NOTE_PACK('F', KPA_NA_SHARP),
  KPA_NOTE_PACK('G', KPA_NA_NATURAL),
  KPA_NOTE_PACK('G', KPA_NA_SHARP),
  KPA_NOTE_PACK('A', KPA_NA_NATURAL),
  KPA_NOTE_PACK('A', KPA_NA_SHARP),
  KPA_NOTE_PACK('B', KPA_NA_NATURAL),
  KPA_NOTE_PACK('_', KPA_NA_NATURAL)
};

static const int c_kpa_note_invalid_index = sizeof(c_kpa_note_table) - 1;

struct kpa_note
{
  //! Name.
  char name;
  //! Octave.
  unsigned octave;
  //! Accidentals.
  kpa_note_accidental_t accidental;
  //! Tune.
  int16_t tune;
};

typedef struct kpa_note* kpa_note_t;

static inline void
kpa_note_init(kpa_note_t note)
{
  note->name = c_kpa_note_invalid_name;
  note->octave = 0;
  note->accidental = KPA_NA_NATURAL;
  note->tune = 0;
}

static inline void
kpa_note_invalidate(kpa_note_t note)
{
  note->name = c_kpa_note_invalid_name;
}

static inline void
kpa_note_set_name(kpa_note_t note, char name)
{
  note->name = name;
}

static inline char
kpa_note_get_name(const kpa_note_t note)
{
  return note->name;
}

static inline void
kpa_note_set_octave(kpa_note_t note, unsigned octave)
{
  note->octave = octave;
}

static inline bool
kpa_note_is_valid(const kpa_note_t note)
{
  return note->name != c_kpa_note_invalid_name;
}

static inline unsigned
kpa_note_get_octave(const kpa_note_t note)
{
  return note->octave;
}

static inline void
kpa_note_set_accidental(kpa_note_t note, kpa_note_accidental_t accidental)
{
  note->accidental = accidental;
}

static inline kpa_note_accidental_t
kpa_note_get_accidental(const kpa_note_t note)
{
  return note->accidental;
}

static inline void
kpa_note_set_tune(kpa_note_t note, int16_t tune)
{
  note->tune = tune;
}

static inline int16_t
kpa_note_get_tune(const kpa_note_t note)
{
  return note->tune;
}

static inline void
kpa_note_set_from_table(kpa_note_t note, int index, unsigned octave)
{
  if (index > c_kpa_note_invalid_index || octave == 0)
    index = c_kpa_note_invalid_index;

  uint8_t note_packed = c_kpa_note_table[index];
  kpa_note_set_name(note, KPA_NOTE_NAME(note_packed));
  kpa_note_set_accidental(note, KPA_NOTE_ACCD(note_packed));
  kpa_note_set_octave(note, octave);
}

#endif
