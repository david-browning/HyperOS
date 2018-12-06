#pragma once
#include "annotations.h"

static constexpr size_t MAX_PARTITIONS_PER_MBR = 4;
static constexpr size_t BOOTSTRAP_CODE_SIZE = 446;
static constexpr size_t PARTITION_TABLE_SIGNATURE_BYTE1 = 0x55;
static constexpr size_t PARTITION_TABLE_SIGNATURE_BYTE2 = 0xAA;
static constexpr size_t GPT_PARTITION_ENTRY_NAME_SIZE = 36;

/*
Structure of a partition table entry.
The size is 16 bytes.
*/
#pragma pack(1)
struct mbr_partition_table_entry_t
{
   uint8_t  BootIND;           //Boot magic number (0x80 if bootable)
   uint8_t  Start_Head;        //Start of partition in CHS 
   uint8_t  Start_Sector;
   uint8_t  Start_Cylinder;
   uint8_t  Part_Type;         //Type of partition (0x81 is minix)
   uint8_t  End_Head;          //End of partition in CHS 
   uint8_t  End_Sector;
   uint8_t  End_Cylinder;
   uint32_t FirstSector_l;    //First sector (LBA addressing) 
                              //gives the first absolute sector number of the 
                              //partition. The sector numbers are relative to 
                              //the beginning of the disk, not the partition.
                              //lFirst + size − 1 gives last.
   uint32_t PartSize;         //size of partition (in sector)
};
#pragma pack()

/*
This is the first sector of every disk or disk partition.
*/
#pragma pack(1)
struct mbr_boot_sector_t
{
   char BootstrapCode[BOOTSTRAP_CODE_SIZE];
   mbr_partition_table_entry_t ParitionEntries[MAX_PARTITIONS_PER_MBR];
   uint8_t BootSignature1;
   uint8_t BootSignature2;
};
#pragma pack()

//The GPT header exists in LBA 1. LBA 0 contains an MBR 
#pragma pack(1)
struct gpt_partition_entry
{
   guid_t PartitionTypeGUID;
   guid_t GUID;
   uint64_t StartLBA;
   uint64_t EndingLBA;
   uint64_t Attributes;
   char16_t PartitionName[GPT_PARTITION_ENTRY_NAME_SIZE];
};
#pragma pack()

#pragma pack(1)
struct gpt_header_t
{
   uint64_t Signature;
   uint32_t Revision;
   uint32_t HeaderSize;
   uint32_t HeaderCRC32;
   uint32_t Reserved;
   uint64_t MyLBA;
   uint64_t AlternateLBA;
   uint64_t FirstUsableLBA;
   uint64_t LastUsableLBA;
   guid_t GUID;
   uint64_t PartitionEntryLBA;
   uint32_t NumPartitionEntries;
   uint32_t PartitionEntrySize;
   uint32_t PartitionEntryArrayCRC32;
   //Reserved Data
};
#pragma pack()

constexpr bool MBRParitionTableIsValid(const mbr_boot_sector_t* table_p)
{
   return (table_p->BootSignature1 == PARTITION_TABLE_SIGNATURE_BYTE1) &&
      (table_p->BootSignature2 == PARTITION_TABLE_SIGNATURE_BYTE2);
}

constexpr bool MBRPartitionIsBootable(const mbr_partition_table_entry_t* part_p)
{
   return (part_p->BootIND & 0x80) == 0x80;
}

