#include "interrupts.h"

void printf(const char* str);

InterruptManager::GateDescriptor InterruptManager::interruptDescriptorTable[256];

void InterruptManager::SetInterruptDescriptorTableEntry(
        uint8_t interruptNumber,
        uint16_t codeSegmentSelectorOffset,
        void (*handler)(),
        uint8_t DescriptorPrivilegeLevel,
        uint8_t descriptorType)
{
  printf("\nGDT Entry Start\n");
  interruptDescriptorTable[interruptNumber].handlerAddressLowBits = ((uint32_t)handler) & 0xFFFF;
  interruptDescriptorTable[interruptNumber].handlerAddressHighBits = (((uint32_t)handler) >> 16) & 0xFFFF;
  interruptDescriptorTable[interruptNumber].gdt_codeSegmentSelector = codeSegmentSelectorOffset;

  const uint8_t IDT_DESC_PRESENT = 0x80;
  interruptDescriptorTable[interruptNumber].access_rights = IDT_DESC_PRESENT | descriptorType | (DescriptorPrivilegeLevel << 5);
  interruptDescriptorTable[interruptNumber].reserved = 0;
  printf("\nGDT Entry End\n");
}

InterruptManager::InterruptManager(GlobalDescriptorTable* gdt)
  : picMasterCommand(0x20),
    picMasterData(0x21),
    picWorkerCommand(0xA0),
    picWorkerData(0xA1)
{
  uint16_t CodeSegment = gdt->CodeSegmentSelector();
  const uint8_t IDT_INTERRUPT_GATE = 0xE;

  for(uint16_t i = 0; i < 256; i++) {
    SetInterruptDescriptorTableEntry(i, CodeSegment, &InterruptIgnore, 0, IDT_INTERRUPT_GATE);
  }
  SetInterruptDescriptorTableEntry(0x20, CodeSegment, &HandleInterruptRequest0x00, 0, IDT_INTERRUPT_GATE);
  SetInterruptDescriptorTableEntry(0x21, CodeSegment, &HandleInterruptRequest0x01, 0, IDT_INTERRUPT_GATE);

  picMasterCommand.Write(0x11);
  picWorkerCommand.Write(0x11);

  picMasterData.Write(0x20);
  picWorkerData.Write(0x20);

  picMasterData.Write(0x04);
  picWorkerData.Write(0x02);

  picMasterData.Write(0x01);
  picWorkerData.Write(0x01);

  picMasterData.Write(0x00);
  picWorkerData.Write(0x00);

  InterruptDescriptorTablePointer idt;
  idt.size = 256 * sizeof(GateDescriptor) - 1;
  idt.base = (uint32_t)interruptDescriptorTable;
  asm volatile("lidt %0" : : "m" (idt));
}

void InterruptManager::InterruptIgnore() {

}

void InterruptManager::HandleInterruptRequest0x01() {
 printf("Handle 0x01"); 
}

void InterruptManager::HandleInterruptRequest0x00() {
 printf("Handle 0x00"); 
}

InterruptManager::~InterruptManager()
{
}


void InterruptManager::Activate()
{
  printf("\nStart activate\n");
  asm("sti");
  printf("Done activate\n");
}

uint32_t InterruptManager::HandleInterrupt(uint8_t interrupt, uint32_t esp)
{
  printf("start handle interrupt");
  //const char* debugString = " INTERRUPT";
  char* foo = "INTERRUPT 0x00";
  char* hex = "0123456789ABCDEF";

  foo[12] = hex[(interrupt >> 4) & 0xF];
  foo[13] = hex[interrupt & 0xF];
  printf(foo);

  return esp;
}
