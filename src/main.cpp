#include <cstdlib>
#include "Serial.hpp"
#include "FbvParser.hpp"

#include "Fbv.hpp"

static Serial uart_fbv("/dev/ttyUSB0");
static Fbv fbv(uart_fbv);


// #define NOTE_PAK(note, flat) (note | flat << 7)
// #define NOTE_NAM(note) (note & 0x7f)
// #define NOTE_FLT(note) ((note >> 7) != 0)

// const static uint8_t c_fbv_note_table[] =
// {
//   NOTE_PAK('C', 0),
//   NOTE_PAK('D', 1),
//   NOTE_PAK('D', 0),
//   NOTE_PAK('E', 1),
//   NOTE_PAK('E', 0),
//   NOTE_PAK('F', 0),
//   NOTE_PAK('F', 1),
//   NOTE_PAK('G', 0),
//   NOTE_PAK('A', 1),
//   NOTE_PAK('A', 0),
//   NOTE_PAK('B', 1),
//   NOTE_PAK('B', 0),
//   NOTE_PAK(' ', 0)
// };


// void
// sendMidiChannel(uint8_t group, uint8_t channel_number, uint8_t channel_char)
// {
//   // FBV_UART_TxBufferPut(0xF0); //header
//   // FBV_UART_TxBufferPut(0x05); //size
//   // FBV_UART_TxBufferPut(0x08); // Channel command
//   // FBV_UART_TxBufferPut(group);  // group (F/U)
//   // FBV_UART_TxBufferPut(0x20);  // 'space'
//   // FBV_UART_TxBufferPut(nr); // Channel number
//   // FBV_UART_TxBufferPut(ch); // Channel char

//   tx_frame.setCommandCode(0x08);
//   tx_frame.setPayloadLength(4);
//   tx_frame.setPayloadByte(0, group);
//   tx_frame.setPayloadByte(1, 0x20);
//   tx_frame.setPayloadByte(2, channel_number);
//   tx_frame.setPayloadByte(3, channel_char);
//   writeFrame();

//   tx_frame.setCommandCode(0x20);
//   tx_frame.setPayloadLength(1);
//   tx_frame.setPayloadByte(0, 0x00);
//   writeFrame();

//   // FBV_UART_TxBufferPut(0xF0); //header
//   // FBV_UART_TxBufferPut(0x02); //size
//   // FBV_UART_TxBufferPut(0x20); // Channel command
//   // FBV_UART_TxBufferPut(0x00);  // 0 = off, 1 = on
// }

// void
// sendTunerNote(char note)
// {
//   dbg("tunerNote: '%c'\r\n", note);
//   // FBV_UART_TxBufferPut(0xF0); //header
//   // FBV_UART_TxBufferPut(0x05); //size
//   // FBV_UART_TxBufferPut(0x08); // Channel command
//   // FBV_UART_TxBufferPut(0x20);  // group (F/U)
//   // FBV_UART_TxBufferPut(0x20);  // 'space'
//   // FBV_UART_TxBufferPut(0x20); // Channel number
//   // FBV_UART_TxBufferPut(0x20); // Channel char

//   // FBV_UART_TxBufferPut(0xF0); //header
//   // FBV_UART_TxBufferPut(0x02); //size
//   // FBV_UART_TxBufferPut(0x0C); // note
//   // FBV_UART_TxBufferPut(note);  // ASCII

//   // FBV_UART_TxBufferPut(0xF0); //header
//   // FBV_UART_TxBufferPut(0x02); //size
//   // FBV_UART_TxBufferPut(0x20); // Channel command
//   // FBV_UART_TxBufferPut(flat);  // 0 = off, 1 = on

//   tx_frame.setCommandCode(0x08);
//   tx_frame.setPayloadLength(4);
//   tx_frame.setPayloadByte(0, 0x20);
//   tx_frame.setPayloadByte(1, 0x20);
//   tx_frame.setPayloadByte(2, 0x20);
//   tx_frame.setPayloadByte(3, 0x20);
//   writeFrame();

//   // Note.
//   tx_frame.setCommandCode(0x0C);
//   tx_frame.setPayloadLength(1);
//   tx_frame.setPayloadByte(0, note);
//   writeFrame();
// }

// void
// setFlat(bool value)
// {
//   tx_frame.setCommandCode(FBV_SET_FLAT);
//   tx_frame.setPayloadLength(1);
//   tx_frame.setPayloadByte(0, value);
//   writeFrame();
// }

// void
// setLed(uint8_t led, bool status)
// {
//   tx_frame.setCommandCode(FBV_SET_LED);
//   tx_frame.setPayloadLength(2);
//   tx_frame.setPayloadByte(0, led);
//   tx_frame.setPayloadByte(1, status);
//   writeFrame();
// }

// void
// requestBoardType(void)
// {
//   tx_frame.setCommandCode(0x01);
//   tx_frame.setPayloadLength(1);
//   tx_frame.setPayloadByte(0, 0x00);
//   writeFrame();
// }

// void
// setTitle(const char* title)
// {
//   tx_frame.setCommandCode(0x10);
//   tx_frame.setPayloadLength(18);
//   tx_frame.setPayloadByte(0, 0x00);
//   tx_frame.setPayloadByte(1, 16);

//   for (int i = 0; i < 16; ++i)
//   {
//     tx_frame.setPayloadByte(2 + i, title[i]);
//   }

//   writeFrame();
// }

// void
// setTunerNeedle()
// {
//   // const char* tuneString;
//   // if (note->name == ' ')
//   //   tuneString = "I              I";

//   // else if (tuneValue > 40)
//   //   tuneString = "I        ( ( ( I";
//   // else if (tuneValue > 25)
//   //   tuneString = "I        ( (   I";
//   // else if (tuneValue > 10)
//   //   tuneString = "I        (     I";
//   // else if (tuneValue > 2)
//   //   tuneString = "I      **(     I";
//   // else if ((tuneValue <= 2) && (tuneValue >= -2))
//   //   tuneString = "I      **      I";
//   // else if (tuneValue >= -10)
//   //   tuneString = "I     )**      I";
//   // else if (tuneValue >= -25)
//   //   tuneString = "I   ) )        I";
//   // else
//   //   tuneString = "I ) ) )        I";
// }

void*
loop(void* a)
{
  (void)a;
  while (true)
  {
    fbv.update();
  }

  return NULL;
}


int
main(void)
{
  pthread_t thread;
  if (pthread_create(&thread, NULL, loop, NULL))
  {
    fprintf(stderr, "Error creating thread\n");
    return 1;
  }

  usleep(2000000);

  while (true)
  {
    // fbv.setTitle("I              I");
    // usleep(100000);
    // fbv.setTitle("I        ( ( ( I");
    // usleep(100000);
    // fbv.setTitle("I        ( (   I");
    // usleep(100000);
    // fbv.setTitle("I        (     I");
    // usleep(100000);
    fbv.setDisplayCharacter('A');
    //fbv.setTitle("I      **(     I");
    // usleep(100000);
    //fbv.setTitle("I      **      I");
    // fbv.setTitle("I     )**      I");
    // usleep(100000);
    fbv.setTitle("I   ) )        I");
    // usleep(100000);
    // fbv.setTitle("I ) ) )        I");
    // usleep(100000);
    usleep(100000);
  }

  return 0;
}
