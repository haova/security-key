#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <hidapi/hidapi.h>

#define USB_VID 0xBA0C
#define USB_PID 0x0001

int main(void)
{
  int res;
  unsigned char buf[65]; // first byte = ReportID, rest = payload
  hid_device *handle;

  // Replace with your device VID & PID
  unsigned short vendor_id = USB_VID;
  unsigned short product_id = USB_PID;

  // Initialize hidapi
  if (hid_init())
    return -1;

  // Open the device using VID, PID
  handle = hid_open(vendor_id, product_id, NULL);
  if (!handle)
  {
    printf("Unable to open device\n");
    return 1;
  }

  // Prepare output report
  buf[0] = 0x00; // Report ID (0x00 if not used)
  buf[1] = 0xAA; // Example data
  buf[2] = 0xBB;
  buf[3] = 0xCC;
  buf[4] = 0xDD;

  // Send output report
  res = hid_write(handle, buf, sizeof(buf));
  if (res < 0)
  {
    printf("Error: %ls\n", hid_error(handle));
  }
  else
  {
    printf("Sent %d bytes\n", res);
  }

  // Close device
  hid_close(handle);
  hid_exit();

  return 0;
}
