#ifndef FBV_HPP_INCLUDED_
#define FBV_HPP_INCLUDED_

#include <stdint.h>

#define FBV_NOTE_PAK(note, flat) (note | flat << 7)
#define FBV_NOTE_NAM(note_pak)   (note_pak & 0x7f)
#define FBV_NOTE_FLT(note_pak)   ((note_pak >> 7) != 0)

class Fbv
{
public:
  Fbv(Serial& uart):
    m_uart(uart),
    m_connected(false)
  {

  }

  void
  setDisplayCharacter(char character)
  {
    m_tx_frame.setCommandCode(0x08);
    m_tx_frame.setPayloadLength(4);
    m_tx_frame.setPayloadByte(0, 0x20);
    m_tx_frame.setPayloadByte(1, 0x20);
    m_tx_frame.setPayloadByte(2, 0x20);
    m_tx_frame.setPayloadByte(3, 0x20);
    writeFrame();

    m_tx_frame.setCommandCode(0x0C);
    m_tx_frame.setPayloadLength(1);
    m_tx_frame.setPayloadByte(0, character);
    writeFrame();
  }

  void
  setFlat(bool value)
  {
    m_tx_frame.setCommandCode(FBV_SET_FLAT);
    m_tx_frame.setPayloadLength(1);
    m_tx_frame.setPayloadByte(0, value);
    writeFrame();
  }

  void
  setLed(uint8_t led, bool status)
  {
    m_tx_frame.setCommandCode(FBV_SET_LED);
    m_tx_frame.setPayloadLength(2);
    m_tx_frame.setPayloadByte(0, led);
    m_tx_frame.setPayloadByte(1, status);
    writeFrame();
  }

  void
  setTunerNeedle()
  {
    // const char* tuneString;
    // if (note->name == ' ')
    //   tuneString = "I              I";

    // else if (tuneValue > 40)
    //   tuneString = "I        ( ( ( I";
    // else if (tuneValue > 25)
    //   tuneString = "I        ( (   I";
    // else if (tuneValue > 10)
    //   tuneString = "I        (     I";
    // else if (tuneValue > 2)
    //   tuneString = "I      **(     I";
    // else if ((tuneValue <= 2) && (tuneValue >= -2))
    //   tuneString = "I      **      I";
    // else if (tuneValue >= -10)
    //   tuneString = "I     )**      I";
    // else if (tuneValue >= -25)
    //   tuneString = "I   ) )        I";
    // else
    //   tuneString = "I ) ) )        I";
  }

  void
  update(void)
  {
    uint8_t byte = 0;
    if (!m_uart.read(&byte))
      return;

    if (!m_parser.parse(m_rx_frame, byte))
      return;

    switch (m_rx_frame.getCommandCode())
    {
      case FBV_CCODE_REQ_FST:
        if (!m_connected)
        {
          onConnection();
          m_connected = true;
        }
        break;

      case FBV_CCODE_REQ_SEC:
        requestBoardType();
        break;

      case FBV_CCODE_BTN:
        onButton();
        break;

      case FBV_CCODE_EXP:
        onPedal();
        break;

      default:
        dbg("Unknown: %02X\r\n", m_rx_frame.getCommandCode());
        break;
    }
  }

  void
  setTitle(const char* title)
  {
    m_tx_frame.setCommandCode(0x10);
    m_tx_frame.setPayloadLength(18);
    m_tx_frame.setPayloadByte(0, 0x00);
    m_tx_frame.setPayloadByte(1, 16);

    for (int i = 0; i < 16; ++i)
    {
      m_tx_frame.setPayloadByte(2 + i, title[i]);
    }

    writeFrame();
  }


private:
  const static uint8_t c_fbv_note_table[];

  //! Parser.
  FbvParser m_parser;
  //! Transmit frame.
  FbvFrame m_tx_frame;
  //! Receive frame.
  FbvFrame m_rx_frame;
  //! UART handle.
  Serial& m_uart;
  //! True if connected, false otherwise.
  bool m_connected;


  void
  requestBoardType(void)
  {
    m_tx_frame.setCommandCode(0x01);
    m_tx_frame.setPayloadLength(1);
    m_tx_frame.setPayloadByte(0, 0x00);
    writeFrame();
  }

  void
  writeFrame(void)
  {
    dbg("W: ");

    int frame_size = m_tx_frame.getFrameSize();
    const uint8_t* frame_data = m_tx_frame.getFrame();

    for (int i = 0; i < frame_size; ++i)
    {
      dbg("%02X ", frame_data[i]);
      m_uart.write(frame_data[i]);
    }
    dbg("\r\n");
  }

  void
  onConnection(void)
  {
    dbg("fbv: connected\r\n");
    setDisplayCharacter(' ');
    setFlat(false);
    setLed(0x20, 0);
    setLed(0x30, 0);
    setLed(0x40, 0);
    setLed(0x50, 0);
  }

  void
  onButton(void)
  {
    unsigned btn_addr = m_rx_frame.getPayloadByte(0);
    unsigned btn = (btn_addr >> 4) - 2;
    unsigned value = m_rx_frame.getPayloadByte(1);
    dbg("fbv: button = %02X | %d %u\n", btn_addr, btn, value);
  }

  void
  onPedal(void)
  {
    unsigned value = m_rx_frame.getPayloadByte(1);
    dbg("fbv: pedal = %u\n", value);
  }
};

// static const Fbv::c_fbv_note_table = {
//     FBV_NOTE_PAK('C', 0),
//     FBV_NOTE_PAK('D', 1),
//     FBV_NOTE_PAK('D', 0),
//     FBV_NOTE_PAK('E', 1),
//     FBV_NOTE_PAK('E', 0),
//     FBV_NOTE_PAK('F', 0),
//     FBV_NOTE_PAK('F', 1),
//     FBV_NOTE_PAK('G', 0),
//     FBV_NOTE_PAK('A', 1),
//     FBV_NOTE_PAK('A', 0),
//     FBV_NOTE_PAK('B', 1),
//     FBV_NOTE_PAK('B', 0),
//     FBV_NOTE_PAK(' ', 0)
//   };


#endif