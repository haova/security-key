#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

// configure serial port
int configure_serial(int fd, int speed) {
  struct termios tty;

  if (tcgetattr(fd, &tty) != 0) {
    perror("tcgetattr");
    return -1;
  }

  cfsetospeed(&tty, speed);
  cfsetispeed(&tty, speed);

  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8-bit chars
  tty.c_iflag &= ~IGNBRK;                     // disable break processing
  tty.c_lflag = 0;                            // no signaling chars, no echo
  tty.c_oflag = 0;                            // no remapping, no delays
  tty.c_cc[VMIN] = 1;                         // read blocks until 1 char
  tty.c_cc[VTIME] = 1;                        // 0.1s read timeout

  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // no software flow control
  tty.c_cflag |= (CLOCAL | CREAD);        // ignore modem, enable read
  tty.c_cflag &= ~(PARENB | PARODD);      // no parity
  tty.c_cflag &= ~CSTOPB;                 // 1 stop bit
  tty.c_cflag &= ~CRTSCTS;                // no hardware flow control

  if (tcsetattr(fd, TCSANOW, &tty) != 0) {
    perror("tcsetattr");
    return -1;
  }

  return 0;
}

int main() {
  const char *portname = "/dev/ttyACM2"; // change as needed
  int fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
  if (fd < 0) {
    perror("open");
    return 1;
  }

  if (configure_serial(fd, B115200) != 0) {
    close(fd);
    return 1;
  }

  // Write data
  const char *msg = "Hello device!\n";
  int n = write(fd, msg, strlen(msg));
  if (n < 0) {
    perror("write");
  } else {
    printf("Wrote %d bytes: %s", n, msg);
  }

  // Read data
  char buf[100];
  n = read(fd, buf, sizeof(buf) - 1);
  if (n < 0) {
    perror("read");
  } else {
    buf[n] = '\0';
    printf("Read %d bytes: %s\n", n, buf);
  }

  close(fd);
  return 0;
}
