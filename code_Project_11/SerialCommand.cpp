/*
 By:Farkad Adnan
 https://linktr.ee/farkadadnan
 E-mail: farkad.hpfa95@gmail.com
 inst : farkadadnan
 #farkadadnan , #farkad_adnan , فرقد عدنان#
 FaceBook: كتاب عالم الاردوينو
 inst : arduinobook
 #كتاب_عالم_الاردوينو  #كتاب_عالم_الآردوينو 
 */
#include "SerialCommand.h"

SerialCommand::SerialCommand()
  : commandList(NULL),
    commandCount(0),
    defaultHandler(NULL),
    term('\r'),      
    last(NULL)
{
  strcpy(delim, ","); // strtok_r needs a null-terminated string
  clearBuffer();
}

void SerialCommand::addCommand(const char *command, void (*function)()) {
  #ifdef SERIALCOMMAND_DEBUG
    SerialUSB.print("Adding command (");
    SerialUSB.print(commandCount);
    SerialUSB.print("): ");
    SerialUSB.println(command);
  #endif

  commandList = (SerialCommandCallback *) realloc(commandList, (commandCount + 1) * sizeof(SerialCommandCallback));
  strncpy(commandList[commandCount].command, command, SERIALCOMMAND_MAXCOMMANDLENGTH);
  commandList[commandCount].function = function;
  commandCount++;
}

void SerialCommand::setDefaultHandler(void (*function)(const char *)) {
  defaultHandler = function;
}


void SerialCommand::readSerial() {
  while (SerialUSB.available() > 0) {
    char inChar = SerialUSB.read();   // Read single available character, there may be more waiting
    #ifdef SERIALCOMMAND_DEBUG
      SerialUSB.print(inChar);   // Echo back to serial stream
    #endif

    if (inChar == term) {     // Check for the terminator (default '\r') meaning end of command
      #ifdef SERIALCOMMAND_DEBUG
        SerialUSB.print("Received: ");
        SerialUSB.println(buffer);
      #endif

      char *command = strtok_r(buffer, delim, &last);   // Search for command at start of buffer
      if (command != NULL) {
        boolean matched = false;
        for (int i = 0; i < commandCount; i++) {
          #ifdef SERIALCOMMAND_DEBUG
            SerialUSB.print("Comparing [");
            SerialUSB.print(command);
            SerialUSB.print("] to [");
            SerialUSB.print(commandList[i].command);
            SerialUSB.println("]");
          #endif

          // Compare the found command against the list of known commands for a match
          if (strncmp(command, commandList[i].command, SERIALCOMMAND_MAXCOMMANDLENGTH) == 0) {
            #ifdef SERIALCOMMAND_DEBUG
              SerialUSB.print("Matched Command: ");
              SerialUSB.println(command);
            #endif

            // Execute the stored handler function for the command
            (*commandList[i].function)();
            matched = true;
            break;
          }
        }
        if (!matched && (defaultHandler != NULL)) {
          (*defaultHandler)(command);
        }
      }
      clearBuffer();
    }
    else if (isprint(inChar)) {     // Only printable characters into the buffer
      if (bufPos < SERIALCOMMAND_BUFFER) {
        buffer[bufPos++] = inChar;  // Put character into buffer
        buffer[bufPos] = '\0';      // Null terminate
      } else {
        #ifdef SERIALCOMMAND_DEBUG
          SerialUSB.println("Line buffer is full - increase SERIALCOMMAND_BUFFER");
        #endif
      }
    }
  }
}

void SerialCommand::clearBuffer() {
  buffer[0] = '\0';
  bufPos = 0;
}


char *SerialCommand::next() {
  return strtok_r(NULL, delim, &last);
}
