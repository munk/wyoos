#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H

#include "types.h"
#include "port.h"
#include "gdt.h"

class InterruptManager {
 public:
  InterruptManager(GlobalDescriptorTable* gdt);
  ~InterruptManager();

  static void Activate();

  static uint32_t HandleInterrupt(uint8_t interruptNumber, uint32_t esp );

  static void InterruptIgnore();
  static void HandleInterruptRequest();
  static void HandleInterruptRequest0x00();
  static void HandleInterruptRequest0x01();

 protected:
  struct GateDescriptor {
    uint16_t handlerAddressLowBits;
    uint16_t gdt_codeSegmentSelector;
    uint8_t reserved;
    uint8_t access_rights;
    uint16_t handlerAddressHighBits;
  } __attribute__((packed));

  static GateDescriptor interruptDescriptorTable[256];

  struct InterruptDescriptorTablePointer
  {
    uint16_t size;
    uint32_t base;
  } __attribute__((packed));

  static void SetInterruptDescriptorTableEntry(
                                               uint8_t interruptNumber,
                                               uint16_t codeSegmentSelectorOffset,
                                               void (*handler)(),
                                               uint8_t DescriptorPrivilegeLevel,
                                               uint8_t DescriptorType);

  Port8BitSlow picMasterCommand;
  Port8BitSlow picMasterData;
  Port8BitSlow picWorkerCommand;
  Port8BitSlow picWorkerData;

};

#endif
