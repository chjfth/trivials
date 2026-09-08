// ==== [2026-08-30] Code by ChatGPT ====
// EDDINFO.CPP
//
// Enumerate BIOS INT 13h disk drives and obtain their sizes
// through the INT 13h Extensions interface.
//
// AH=41h  - Check INT 13h Extensions
// AH=48h  - Extended Get Drive Parameters
//
// Target:
//     16-bit DOS real mode
//     Compile using Open Watcom 1.9
//
// Compile:
//     wpp -0 -ms eddinfo.cpp
//
// Link:
//     wlink system dos name eddinfo file eddinfo
//
// Or simply:
//     wcl -0 -ms -l=dos eddinfo.cpp
//

#include <stdio.h>
#include <string.h>
#include <dos.h>

typedef unsigned __int64 Uint64;

//
// EDD AH=48h parameter buffer.
//
// Layout defined by the BIOS Enhanced Disk Drive specification:
//
//   00h  WORD   buffer size
//   02h  WORD   information flags
//   04h  DWORD  cylinders
//   08h  DWORD  heads
//   0Ch  DWORD  sectors/track
//   10h  QWORD  total number of sectors
//   18h  WORD   bytes per sector
//
// We only need fields through 19h.
//

#pragma pack(1)

struct EDD_PARAMS
{
	unsigned short size;
	unsigned short flags;

	unsigned long cylinders;
	unsigned long heads;
	unsigned long sectors_per_track;

	unsigned long total_sectors_lo;
	unsigned long total_sectors_hi;

	unsigned short bytes_per_sector;

	unsigned char reserved[16];
};

#pragma pack()


//
// Check whether a BIOS drive supports INT 13h Extensions.
//
// INT 13h AH=41h
//
// Input:
//     DL = BIOS drive number
//
// Output:
//     CF clear
//     BX = AA55h
//     CX bit 0 = extended disk access functions supported
//
// Returns:
//     1 = supported
//     0 = not supported
//

int CheckEDD(unsigned char drive)
{
	union REGS r;

	r.h.ah = 0x41;
	r.h.dl = drive;

	r.x.bx = 0x55AA;

	int86(0x13, &r, &r);

	//
	// Carry flag means the BIOS did not support the function.
	//
	if (r.x.cflag)
		return 0;

	//
	// BIOS must return AA55h in BX.
	//
	if (r.x.bx != 0xAA55)
		return 0;

	//
	// CX bit 0 indicates support for extended disk access.
	//
	if ((r.x.cx & 0x0001) == 0)
		return 0;

	return 1;
}


//
// Get EDD parameters using INT 13h AH=48h.
//
// Input:
//     DL = BIOS drive number
//     DS:SI -> EDD_PARAMS buffer
//
// Returns:
//     0       success
//     BIOS AH error code on failure
//

unsigned char GetEDDParameters(
	unsigned char drive,
	EDD_PARAMS *params)
{
	union REGS r;
	struct SREGS s;

	//
	// Clear the complete buffer.
	//
	memset(params, 0, sizeof(EDD_PARAMS));

	//
	// Tell the BIOS how large our buffer is.
	//
	params->size = sizeof(EDD_PARAMS);

	//
	// Obtain current data-segment value.
	//
	segread(&s);

	//
	// AH=48h
	//
	r.x.ax = 0x4800;

	//
	// BIOS drive number.
	//
	r.h.dl = drive;

	//
	// SI = offset of our buffer.
	//
	r.x.si = FP_OFF(params);

	//
	// DS = segment containing our buffer.
	//
	s.ds = FP_SEG(params);

	//
	// Call BIOS INT 13h.
	//
	int86x(0x13, &r, &r, &s);

	//
	// BIOS error?
	//
	if (r.x.cflag)
		return r.h.ah;

	return 0;
}


//
// Print an unsigned 64-bit value represented by
// two unsigned 32-bit values.
//
// The value is:
//
//     HI:LO
//
// and is stored little-endian in the EDD buffer.
//

void PrintHex64(
	unsigned long hi,
	unsigned long lo)
{
	printf(
		"0x%08lX`%08lX", hi, lo);
}


//
// Print an unsigned 64-bit value in decimal.
//
// Open Watcom's 16-bit environment does not require us
// to use "unsigned long long" here, so implement the
// decimal conversion manually.
//

void PrintDecimal64(
	unsigned long hi,
	unsigned long lo)
{
	char digits[21];

	int pos;

	unsigned long qhi;
	unsigned long qlo;

	unsigned long remainder;

	pos = 20;

	digits[pos] = '\0';

	qhi = hi;
	qlo = lo;

	//
	// Special case zero.
	//
	if (qhi == 0 && qlo == 0)
	{
		printf("0");
		return;
	}

	//
	// Repeatedly divide the 64-bit number by 10.
	//
	while (qhi != 0 || qlo != 0)
	{
		unsigned long high_quotient;
		unsigned long high_remainder;

		unsigned int low_high;
		unsigned int low_low;

		unsigned long n;
		unsigned long q1;
		unsigned long r1;

		//
		// Divide the high 32 bits by 10.
		//
		high_quotient = qhi / 10;
		high_remainder = qhi % 10;

		//
		// Now divide:
		//
		//     (high_remainder << 32) | qlo
		//
		// by 10.
		//
		// Do this using two 16-bit pieces.
		//

		low_high = (unsigned int)(qlo >> 16);
		low_low  = (unsigned int)(qlo & 0xFFFF);

		//
		// Process upper 16 bits.
		//
		n = high_remainder * 65536UL + low_high;

		q1 = n / 10;
		r1 = n % 10;

		//
		// Process lower 16 bits.
		//
		n = r1 * 65536UL + low_low;

		qlo = (q1 << 16) | (n / 10);

		remainder = n % 10;

		qhi = high_quotient;

		//
		// Store the decimal digit.
		//
		if (pos == 0)
			break;

		digits[--pos] =
			(char)('0' + remainder);
	}

	printf("%s", &digits[pos]);
}


//
// Main.
//

int main()
{
	unsigned int drive;

	printf("\n");
	printf("BIOS INT 13h EDD Disk Enumerator\n");
	printf("================================\n");
//	printf("\n");

	//
	// BIOS hard disks conventionally start at 80h.
	//
	// We probe all possible BIOS drive numbers through FFh.
	//
	for (drive = 0x80; drive <= 0xFF; ++drive)
	{
		unsigned char drive_number;
		EDD_PARAMS params;

		unsigned char error;

		drive_number =
			(unsigned char)drive;

		//
		// First ask whether this drive supports
		// INT 13h Extensions.
		//
		if (!CheckEDD(drive_number))
			continue;

		printf(
			"Drive %02Xh\n",
			drive_number);

		//
		// Now obtain the extended drive parameters.
		//
		error =
			GetEDDParameters(
				drive_number,
				&params);

		if (error != 0)
		{
			printf(
				"  AH=48h failed, BIOS error = %02Xh\n",
				error);

			printf("\n");

			continue;
		}

		//
		// Display the geometry returned by BIOS.
		//
		printf(
			"  C/H/S         : %lu/%lu/%lu\n",
			params.cylinders, params.heads, params.sectors_per_track);

		//
		// Display the important value:
		// the 64-bit total sector count.
		//
		printf(
			"  Total sectors : ");
		PrintDecimal64(
			params.total_sectors_hi,
			params.total_sectors_lo);
		printf(" (=");
		PrintHex64(
			params.total_sectors_hi,
			params.total_sectors_lo);
		printf(")\n");

		printf(
			"  Bytes/sector  : %u\n",
			params.bytes_per_sector);

		//
		// Give a convenient capacity in decimal GB.
		// Chj added:
		
		Uint64 total_bytes = 512 *
			(((Uint64)(params.total_sectors_hi)<<32) + params.total_sectors_lo);
//		printf("total bytes: %Lu\n", total_bytes);
		printf(
			"  Disk capacity : %.3g GiB (=%.3g gB)\n", 
			(double)total_bytes/((Uint64)1024*1024*1024),
			(double)total_bytes/((Uint64)1000*1000*1000));
	}

//	printf("Scan complete.\n");
	return 0;
}