#ifndef NOTE_HPP_INCLUDED_
#define NOTE_HPP_INCLUDED_

#include "Debug.hpp"

class Note
{
public:
  enum Accidentals
  {
    ACC_FLAT,
    ACC_SHARP,
    ACC_NATURAL
  };

  char
  getName(void) const
  {
    return m_name;
  }

  void
  setName(char name)
  {
    m_name = name;
  }

  unsigned
  getOctave(void) const
  {
    return m_octave;
  }

  void
  setOctave(unsigned octave)
  {
    m_octave = octave;
  }

  void
  setAccidental(Accidentals accidental)
  {
    m_accidental = accidental;
  }

  void
  setInvalid()
  {
    m_name = ' ';
  }

  bool
  isValid(void) const
  {
    return m_name != ' ';
  }

  bool
  isNatural(void) const
  {
    return m_accidental == ACC_NATURAL;
  }

  bool
  isSharp(void) const
  {
    return m_accidental == ACC_SHARP;
  }

  bool
  isFlat(void) const
  {
    return m_accidental == ACC_FLAT;
  }

  void
  dump(void) const
  {
    dbg("Note: %c%c%u\r\n", m_name, getAccidentalSymbol(), m_octave);
  }

private:
  //! Name of the note.
  char m_name;
  //! Mark note as accidental.
  Accidentals m_accidental;
  //! Octave number.
  unsigned m_octave;

  char
  getAccidentalSymbol(void) const
  {
    switch (m_accidental)
    {
      case ACC_FLAT:
        return 'b';

      case ACC_SHARP:
        return '#';

      default:
        return ' ';
    }
  }

};

#endif
