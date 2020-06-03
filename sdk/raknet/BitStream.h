/// \file
/// \brief This class allows you to write and read native types as a string of bits.  BitStream is used extensively throughout RakNet and is designed to be used by users as well.
///
/// This file is part of RakNet Copyright 2003 Kevin Jenkins.
///
/// Usage of RakNet is subject to the appropriate license agreement.
/// Creative Commons Licensees are subject to the
/// license found at
/// http://creativecommons.org/licenses/by-nc/2.5/
/// Single application licensees are subject to the license found at
/// http://www.rakkarsoft.com/SingleApplicationLicense.html
/// Custom license users are subject to the terms therein.
/// GPL license users are subject to the GNU General Public
/// License as published by the Free
/// Software Foundation; either version 2 of the License, or (at your
/// option) any later version.

#ifndef __BITSTREAM_H
#define __BITSTREAM_H

#include "Export.h"
#include "RakAssert.h"
#include "NetworkTypes.h"

typedef unsigned int RakUInt32;
typedef RakUInt32 BitSize_t;

#ifdef _MSC_VER
#pragma warning( push )
#endif

/// Arbitrary size, just picking something likely to be larger than most packets
#define BITSTREAM_STACK_ALLOCATION_SIZE 256

/// The namespace RakNet is not consistently used.  It's only purpose is to avoid compiler errors for classes whose names are very common.
/// For the most part I've tried to avoid this simply by using names very likely to be unique for my classes.
namespace RakNet
{
	/// This class allows you to write and read native types as a string of bits.  BitStream is used extensively throughout RakNet and is designed to be used by users as well.
	/// \sa BitStreamSample.txt
	class RAK_DLL_EXPORT BitStream
	{
	public:
		/// Default Constructor
		BitStream();

		/// \brief Create the bitstream, with some number of bytes to immediately allocate.
		/// \details There is no benefit to calling this, unless you know exactly how many bytes you need and it is greater than BITSTREAM_STACK_ALLOCATION_SIZE.
		/// In that case all it does is save you one or more realloc calls.
		/// \param[in] initialBytesToAllocate the number of bytes to pre-allocate.
		BitStream(const unsigned int initialBytesToAllocate);

		/// \brief Initialize the BitStream, immediately setting the data it contains to a predefined pointer.
		/// \details Set \a _copyData to true if you want to make an internal copy of the data you are passing. Set it to false to just save a pointer to the data.
		/// You shouldn't call Write functions with \a _copyData as false, as this will write to unallocated memory
		/// 99% of the time you will use this function to cast Packet::data to a bitstream for reading, in which case you should write something as follows:
		/// \code
		/// RakNet::BitStream bs(packet->data, packet->length, false);
		/// \endcode
		/// \param[in] _data An array of bytes.
		/// \param[in] lengthInBytes Size of the \a _data.
		/// \param[in] _copyData true or false to make a copy of \a _data or not.
		BitStream(unsigned char* _data, const unsigned int lengthInBytes, bool _copyData);

		// SAMPSRV (adding this just as a tag for next RakNet upgrade)
		BitStream(RPCParameters* pParams, bool _copyData = false);
		BitStream(Packet* pPacket, bool _copyData = false);
		// SAMPSRV end

		// Destructor
		~BitStream();

		/// Resets the bitstream for reuse.
		void Reset(void);

		/// \brief Write an array or casted stream or raw data.  This does NOT do endian swapping.
		/// \param[in] inputByteArray a byte buffer
		/// \param[in] numberOfBytes the size of \a input in bytes
		void Write(const char* inputByteArray, const unsigned int numberOfBytes);

		/// \brief Read one bitstream to another.
		/// \param[in] numberOfBits bits to read
		/// \param bitStream the bitstream to read into from
		/// \return true on success, false on failure.
		bool Read(BitStream* bitStream, BitSize_t numberOfBits);
		bool Read(BitStream* bitStream);
		bool Read(BitStream& bitStream, BitSize_t numberOfBits);
		bool Read(BitStream& bitStream);

		/// \brief Write one bitstream to another.
		/// \param[in] numberOfBits bits to write
		/// \param bitStream the bitstream to copy from
		void Write(BitStream* bitStream, BitSize_t numberOfBits);
		void Write(BitStream* bitStream);
		void Write(BitStream& bitStream, BitSize_t numberOfBits);
		void Write(BitStream& bitStream);

		/// \brief Sets the read pointer back to the beginning of your data.
		void ResetReadPointer(void);

		/// \brief Sets the write pointer back to the beginning of your data.
		void ResetWritePointer(void);

		/// \brief Write a 0
		void Write0(void);

		/// \brief Write a 1
		void Write1(void);

		/// \brief Reads 1 bit and returns true if that bit is 1 and false if it is 0.
		bool ReadBit(void);

		/// \brief Align the bitstream to the byte boundary and then write the
		/// specified number of bits.  
		/// \details This is faster than WriteBits but
		/// wastes the bits to do the alignment and requires you to call
		/// ReadAlignedBits at the corresponding read position.
		/// \param[in] inByteArray The data
		/// \param[in] numberOfBytesToWrite The size of input.
		void WriteAlignedBytes(const unsigned char* inByteArray, const unsigned int numberOfBytesToWrite);

		// Endian swap bytes already in the bitstream
		void EndianSwapBytes(int byteOffset, int length);

		/// \brief Aligns the bitstream, writes inputLength, and writes input. Won't write beyond maxBytesToWrite
		/// \param[in] inByteArray The data
		/// \param[in] inputLength The size of input.
		/// \param[in] maxBytesToWrite Max bytes to write
		void WriteAlignedBytesSafe(const char* inByteArray, const unsigned int inputLength, const unsigned int maxBytesToWrite);

		/// \brief Read bits, starting at the next aligned bits. 
		/// \details Note that the modulus 8 starting offset of the sequence must be the same as
		/// was used with WriteBits. This will be a problem with packet
		/// coalescence unless you byte align the coalesced packets.
		/// \param[in] inOutByteArray The byte array larger than @em numberOfBytesToRead
		/// \param[in] numberOfBytesToRead The number of byte to read from the internal state
		/// \return true if there is enough byte.
		bool ReadAlignedBytes(unsigned char* inOutByteArray, const unsigned int numberOfBytesToRead);

		/// \brief Reads what was written by WriteAlignedBytesSafe.
		/// \param[in] inOutByteArray The data
		/// \param[in] maxBytesToRead Maximum number of bytes to read
		/// \return true on success, false on failure.
		bool ReadAlignedBytesSafe(char* inOutByteArray, int& inputLength, const int maxBytesToRead);
		bool ReadAlignedBytesSafe(char* inOutByteArray, unsigned int& inputLength, const unsigned int maxBytesToRead);

		/// \brief Same as ReadAlignedBytesSafe() but allocates the memory for you using new, rather than assuming it is safe to write to
		/// \param[in] outByteArray outByteArray will be deleted if it is not a pointer to 0
		/// \return true on success, false on failure.
		bool ReadAlignedBytesSafeAlloc(char** outByteArray, int& inputLength, const unsigned int maxBytesToRead);
		bool ReadAlignedBytesSafeAlloc(char** outByteArray, unsigned int& inputLength, const unsigned int maxBytesToRead);

		/// \brief Write numberToWrite bits from the input source.
		/// \details Right aligned data means in the case of a partial byte, the bits are aligned
		/// from the right (bit 0) rather than the left (as in the normal
		/// internal representation) You would set this to true when
		/// writing user data, and false when copying bitstream data, such
		/// as writing one bitstream to another.
		/// \param[in] inByteArray The data
		/// \param[in] numberOfBitsToWrite The number of bits to write
		/// \param[in] rightAlignedBits if true data will be right aligned
		void WriteBits(const unsigned char* inByteArray, BitSize_t numberOfBitsToWrite, const bool rightAlignedBits = true);

		/// \internal
		/// Set the stream to some initial data.
		void SetData(unsigned char* inByteArray);

		/// \brief Assume the input source points to a native type, compress and write it.
		void WriteCompressed(const unsigned char* inByteArray, const unsigned int size, const bool unsignedData);

		/// \brief Align the next write and/or read to a byte boundary.  
		/// \details This can be used to 'waste' bits to byte align for efficiency reasons It
		/// can also be used to force coalesced bitstreams to start on byte
		/// boundaries so so WriteAlignedBits and ReadAlignedBits both
		/// calculate the same offset when aligning.
		void AlignReadToByteBoundary(void);

		/// \brief Read \a numberOfBitsToRead bits to the output source.
		/// \details alignBitsToRight should be set to true to convert internal
		/// bitstream data to userdata. It should be false if you used
		/// WriteBits with rightAlignedBits false
		/// \param[in] inOutByteArray The resulting bits array
		/// \param[in] numberOfBitsToRead The number of bits to read
		/// \param[in] alignBitsToRight if true bits will be right aligned.
		/// \return true if there is enough bits to read
		bool ReadBits(unsigned char* inOutByteArray, BitSize_t numberOfBitsToRead, const bool alignBitsToRight = true);

		/// \brief Assume the input source points to a compressed native type. Decompress and read it.
		bool ReadCompressed(unsigned char* inOutByteArray, const unsigned int size, const bool unsignedData);

		/// \brief Reallocates (if necessary) in preparation of writing numberOfBitsToWrite
		void AddBitsAndReallocate(const BitSize_t numberOfBitsToWrite);

		/// \internal
		/// \return How many bits have been allocated internally
		BitSize_t GetNumberOfBitsAllocated(void) const;

		/// Write zeros until the bitstream is filled up to \a bytes
		void PadWithZeroToByteLength(unsigned int bytes);

		/// \brief Returns the length in bits of the stream
		inline BitSize_t GetNumberOfBitsUsed(void) const { return GetWriteOffset(); }
		inline BitSize_t GetWriteOffset(void) const { return numberOfBitsUsed; }

		/// \brief Returns the length in bytes of the stream
		inline BitSize_t GetNumberOfBytesUsed(void) const { return BITS_TO_BYTES(numberOfBitsUsed); }

		/// \brief Returns the number of bits into the stream that we have read
		inline BitSize_t GetReadOffset(void) const { return readOffset; }

		/// \brief Returns the number of bits left in the stream that haven't been read
		inline BitSize_t GetNumberOfUnreadBits(void) const { return readOffset > numberOfBitsUsed ? 0 : numberOfBitsUsed - readOffset; }

		/// \brief Sets the read bit index
		void SetReadOffset(const BitSize_t newReadOffset);

		/// \brief Makes a copy of the internal data for you \a _data will point to
		/// the stream. Partial bytes are left aligned.
		/// \param[out] _data The allocated copy of GetData()
		/// \return The length in bits of the stream.
		BitSize_t CopyData(unsigned char** _data) const;

		/// \brief Ignore data we don't intend to read
		/// \param[in] numberOfBits The number of bits to ignore
		void IgnoreBits(const BitSize_t numberOfBits);

		/// \brief Ignore data we don't intend to read
		/// \param[in] numberOfBits The number of bytes to ignore
		void IgnoreBytes(const unsigned int numberOfBytes);

		/// \brief Move the write pointer to a position on the array.
		/// \param[in] offset the offset from the start of the array.
		/// \attention
		/// \details Dangerous if you don't know what you are doing!
		/// For efficiency reasons you can only write mid-stream if your data is byte aligned.
		void SetWriteOffset(const BitSize_t offset);

		/// \brief Use this if you pass a pointer copy to the constructor
		/// *(_copyData==false) and want to overallocate to prevent
		/// reallocation.
		void SetNumberOfBitsAllocated(const BitSize_t lengthInBits);

		/// \brief If we used the constructor version with copy data off, this
		/// *makes sure it is set to on and the data pointed to is copied.
		void AssertCopyData(void);

		// Not inline, won't compile on PC due to winsock include errors
		static bool IsNetworkOrderInternal(void);

		static void ReverseBytes(unsigned char* inByteArray, unsigned char* inOutByteArray, const unsigned int length);
		static void ReverseBytesInPlace(unsigned char* inOutData, const unsigned int length);

		/// \brief Align the next write and/or read to a byte boundary.  
		/// \details This can be used to 'waste' bits to byte align for efficiency reasons It
		/// can also be used to force coalesced bitstreams to start on byte
		/// boundaries so so WriteAlignedBits and ReadAlignedBits both
		/// calculate the same offset when aligning.
		inline void AlignWriteToByteBoundary(void) { numberOfBitsUsed += 8 - (((numberOfBitsUsed - 1) & 7) + 1); }

		/// Gets the data that BitStream is writing to / reading from.
		/// Partial bytes are left aligned.
		/// \return A pointer to the internal state
		inline unsigned char* GetData(void) const { return data; }

		/// \brief Bidirectional serialize/deserialize any integral type to/from a bitstream.  
		/// \details Undefine __BITSTREAM_NATIVE_END if you need endian swapping.
		/// \param[in] writeToBitstream true to write from your data to this bitstream.  False to read from this bitstream and write to your data
		/// \param[in] inOutTemplateVar The value to write
		/// \return true if \a writeToBitstream is true.  true if \a writeToBitstream is false and the read was successful.  false if \a writeToBitstream is false and the read was not successful.
		template <class templateType>
		bool Serialize(bool writeToBitstream, templateType& inOutTemplateVar);

		/// \brief Bidirectional serialize/deserialize any integral type to/from a bitstream. 
		/// \details If the current value is different from the last value
		/// the current value will be written.  Otherwise, a single bit will be written
		/// \param[in] writeToBitstream true to write from your data to this bitstream.  False to read from this bitstream and write to your data
		/// \param[in] inOutCurrentValue The current value to write
		/// \param[in] lastValue The last value to compare against.  Only used if \a writeToBitstream is true.
		/// \return true if \a writeToBitstream is true.  true if \a writeToBitstream is false and the read was successful.  false if \a writeToBitstream is false and the read was not successful.
		template <class templateType>
		bool SerializeDelta(bool writeToBitstream, templateType& inOutCurrentValue, const templateType& lastValue);

		/// \brief Bidirectional version of SerializeDelta when you don't know what the last value is, or there is no last value.
		/// \param[in] writeToBitstream true to write from your data to this bitstream.  False to read from this bitstream and write to your data
		/// \param[in] inOutCurrentValue The current value to write
		/// \return true if \a writeToBitstream is true.  true if \a writeToBitstream is false and the read was successful.  false if \a writeToBitstream is false and the read was not successful.
		template <class templateType>
		bool SerializeDelta(bool writeToBitstream, templateType& inOutCurrentValue);

		/// \brief Bidirectional serialize/deserialize any integral type to/from a bitstream.
		/// \details Undefine __BITSTREAM_NATIVE_END if you need endian swapping.
		/// If you are not using __BITSTREAM_NATIVE_END the opposite is true for types larger than 1 byte
		/// For floating point, this is lossy, using 2 bytes for a float and 4 for a double.  The range must be between -1 and +1.
		/// For non-floating point, this is lossless, but only has benefit if you use less than half the bits of the type
		/// \param[in] writeToBitstream true to write from your data to this bitstream.  False to read from this bitstream and write to your data
		/// \param[in] inOutTemplateVar The value to write
		/// \return true if \a writeToBitstream is true.  true if \a writeToBitstream is false and the read was successful.  false if \a writeToBitstream is false and the read was not successful.
		template <class templateType>
		bool SerializeCompressed(bool writeToBitstream, templateType& inOutTemplateVar);

		/// \brief Bidirectional serialize/deserialize any integral type to/from a bitstream.  
		/// \details If the current value is different from the last value
		/// the current value will be written.  Otherwise, a single bit will be written
		/// For floating point, this is lossy, using 2 bytes for a float and 4 for a double.  The range must be between -1 and +1.
		/// For non-floating point, this is lossless, but only has benefit if you use less than half the bits of the type
		/// If you are not using __BITSTREAM_NATIVE_END the opposite is true for types larger than 1 byte
		/// \param[in] writeToBitstream true to write from your data to this bitstream.  False to read from this bitstream and write to your data
		/// \param[in] inOutCurrentValue The current value to write
		/// \param[in] lastValue The last value to compare against.  Only used if \a writeToBitstream is true.
		/// \return true if \a writeToBitstream is true.  true if \a writeToBitstream is false and the read was successful.  false if \a writeToBitstream is false and the read was not successful.
		template <class templateType>
		bool SerializeCompressedDelta(bool writeToBitstream, templateType& inOutCurrentValue, const templateType& lastValue);

		/// \brief Save as SerializeCompressedDelta(templateType &currentValue, const templateType &lastValue) when we have an unknown second parameter
		/// \return true on data read. False on insufficient data in bitstream
		template <class templateType>
		bool SerializeCompressedDelta(bool writeToBitstream, templateType& inOutTemplateVar);

		/// \brief Bidirectional serialize/deserialize an array or casted stream or raw data.  This does NOT do endian swapping.
		/// \param[in] writeToBitstream true to write from your data to this bitstream.  False to read from this bitstream and write to your data
		/// \param[in] inOutByteArray a byte buffer
		/// \param[in] numberOfBytes the size of \a input in bytes
		/// \return true if \a writeToBitstream is true.  true if \a writeToBitstream is false and the read was successful.  false if \a writeToBitstream is false and the read was not successful.
		bool Serialize(bool writeToBitstream, char* inOutByteArray, const unsigned int numberOfBytes);

		/// \brief Bidirectional serialize/deserialize a normalized 3D vector, using (at most) 4 bytes + 3 bits instead of 12-24 bytes.  
		/// \details Will further compress y or z axis aligned vectors.
		/// Accurate to 1/32767.5.
		/// \param[in] writeToBitstream true to write from your data to this bitstream.  False to read from this bitstream and write to your data
		/// \param[in] x x
		/// \param[in] y y
		/// \param[in] z z
		/// \return true if \a writeToBitstream is true.  true if \a writeToBitstream is false and the read was successful.  false if \a writeToBitstream is false and the read was not successful.
		template <class templateType> // templateType for this function must be a float or double
		bool SerializeNormVector(bool writeToBitstream, templateType& x, templateType& y, templateType& z);

		/// \brief Bidirectional serialize/deserialize a vector, using 10 bytes instead of 12.
		/// \details Loses accuracy to about 3/10ths and only saves 2 bytes, so only use if accuracy is not important.
		/// \param[in] writeToBitstream true to write from your data to this bitstream.  False to read from this bitstream and write to your data
		/// \param[in] x x
		/// \param[in] y y
		/// \param[in] z z
		/// \return true if \a writeToBitstream is true.  true if \a writeToBitstream is false and the read was successful.  false if \a writeToBitstream is false and the read was not successful.
		template <class templateType> // templateType for this function must be a float or double
		bool SerializeVector(bool writeToBitstream, templateType& x, templateType& y, templateType& z);

		/// \brief Bidirectional serialize/deserialize a normalized quaternion in 6 bytes + 4 bits instead of 16 bytes. Slightly lossy.
		/// \param[in] writeToBitstream true to write from your data to this bitstream.  False to read from this bitstream and write to your data
		/// \param[in] w w
		/// \param[in] x x
		/// \param[in] y y
		/// \param[in] z z
		/// \return true if \a writeToBitstream is true.  true if \a writeToBitstream is false and the read was successful.  false if \a writeToBitstream is false and the read was not successful.
		template <class templateType> // templateType for this function must be a float or double
		bool SerializeNormQuat(bool writeToBitstream, templateType& w, templateType& x, templateType& y, templateType& z);

		/// \brief Bidirectional serialize/deserialize an orthogonal matrix by creating a quaternion, and writing 3 components of the quaternion in 2 bytes each.
		/// \details Use 6 bytes instead of 36
		/// Lossy, although the result is renormalized
		/// \return true on success, false on failure.
		template <class templateType> // templateType for this function must be a float or double
		bool SerializeOrthMatrix(
			bool writeToBitstream,
			templateType& m00, templateType& m01, templateType& m02,
			templateType& m10, templateType& m11, templateType& m12,
			templateType& m20, templateType& m21, templateType& m22);

		/// \brief Bidirectional serialize/deserialize numberToSerialize bits to/from the input. 
		/// \details Right aligned data means in the case of a partial byte, the bits are aligned
		/// from the right (bit 0) rather than the left (as in the normal
		/// internal representation) You would set this to true when
		/// writing user data, and false when copying bitstream data, such
		/// as writing one bitstream to another
		/// \param[in] writeToBitstream true to write from your data to this bitstream.  False to read from this bitstream and write to your data
		/// \param[in] inOutByteArray The data
		/// \param[in] numberOfBitsToSerialize The number of bits to write
		/// \param[in] rightAlignedBits if true data will be right aligned
		/// \return true if \a writeToBitstream is true.  true if \a writeToBitstream is false and the read was successful.  false if \a writeToBitstream is false and the read was not successful.
		bool SerializeBits(bool writeToBitstream, unsigned char* inOutByteArray, const BitSize_t numberOfBitsToSerialize, const bool rightAlignedBits = true);

		/// \brief Write any integral type to a bitstream.  
		/// \details Undefine __BITSTREAM_NATIVE_END if you need endian swapping.
		/// \param[in] inTemplateVar The value to write
		template <class templateType>
		void Write(const templateType& inTemplateVar);

		/// \brief Write any integral type to a bitstream.  
		/// \details If the current value is different from the last value
		/// the current value will be written.  Otherwise, a single bit will be written
		/// \param[in] currentValue The current value to write
		/// \param[in] lastValue The last value to compare against
		template <class templateType>
		void WriteDelta(const templateType& currentValue, const templateType& lastValue);

		/// \brief WriteDelta when you don't know what the last value is, or there is no last value.
		/// \param[in] currentValue The current value to write
		template <class templateType>
		void WriteDelta(const templateType& currentValue);

		/// \brief Write any integral type to a bitstream.  
		/// \details Undefine __BITSTREAM_NATIVE_END if you need endian swapping.
		/// If you are not using __BITSTREAM_NATIVE_END the opposite is true for types larger than 1 byte
		/// For floating point, this is lossy, using 2 bytes for a float and 4 for a double.  The range must be between -1 and +1.
		/// For non-floating point, this is lossless, but only has benefit if you use less than half the bits of the type
		/// \param[in] inTemplateVar The value to write
		template <class templateType>
		void WriteCompressed(const templateType& inTemplateVar);

		/// \brief Write any integral type to a bitstream.  
		/// \details If the current value is different from the last value
		/// the current value will be written.  Otherwise, a single bit will be written
		/// For floating point, this is lossy, using 2 bytes for a float and 4 for a double.  The range must be between -1 and +1.
		/// For non-floating point, this is lossless, but only has benefit if you use less than half the bits of the type
		/// If you are not using __BITSTREAM_NATIVE_END the opposite is true for types larger than 1 byte
		/// \param[in] currentValue The current value to write
		/// \param[in] lastValue The last value to compare against
		template <class templateType>
		void WriteCompressedDelta(const templateType& currentValue, const templateType& lastValue);

		/// \brief Save as WriteCompressedDelta(const templateType &currentValue, const templateType &lastValue) when we have an unknown second parameter
		template <class templateType>
		void WriteCompressedDelta(const templateType& currentValue);

		/// \brief Read any integral type from a bitstream.  
		/// \details Define __BITSTREAM_NATIVE_END if you need endian swapping.
		/// \param[in] outTemplateVar The value to read
		/// \return true on success, false on failure.
		template <class templateType>
		bool Read(templateType& outTemplateVar);

		/// \brief Read any integral type from a bitstream.  
		/// \details If the written value differed from the value compared against in the write function,
		/// var will be updated.  Otherwise it will retain the current value.
		/// ReadDelta is only valid from a previous call to WriteDelta
		/// \param[in] outTemplateVar The value to read
		/// \return true on success, false on failure.
		template <class templateType>
		bool ReadDelta(templateType& outTemplateVar);

		/// \brief Read any integral type from a bitstream.  
		/// \details Undefine __BITSTREAM_NATIVE_END if you need endian swapping.
		/// For floating point, this is lossy, using 2 bytes for a float and 4 for a double.  The range must be between -1 and +1.
		/// For non-floating point, this is lossless, but only has benefit if you use less than half the bits of the type
		/// If you are not using __BITSTREAM_NATIVE_END the opposite is true for types larger than 1 byte
		/// \param[in] outTemplateVar The value to read
		/// \return true on success, false on failure.
		template <class templateType>
		bool ReadCompressed(templateType& outTemplateVar);

		/// \brief Read any integral type from a bitstream.  
		/// \details If the written value differed from the value compared against in the write function,
		/// var will be updated.  Otherwise it will retain the current value.
		/// the current value will be updated.
		/// For floating point, this is lossy, using 2 bytes for a float and 4 for a double.  The range must be between -1 and +1.
		/// For non-floating point, this is lossless, but only has benefit if you use less than half the bits of the type
		/// If you are not using __BITSTREAM_NATIVE_END the opposite is true for types larger than 1 byte
		/// ReadCompressedDelta is only valid from a previous call to WriteDelta
		/// \param[in] outTemplateVar The value to read
		/// \return true on success, false on failure.
		template <class templateType>
		bool ReadCompressedDelta(templateType& outTemplateVar);

		/// \brief Write a normalized 3D vector, using (at most) 4 bytes + 3 bits instead of 12-24 bytes.  
		/// \details Will further compress y or z axis aligned vectors.
		/// Accurate to 1/32767.5.
		/// \param[in] x x
		/// \param[in] y y
		/// \param[in] z z
		template <class templateType> // templateType for this function must be a float or double
		void WriteNormVector(templateType x, templateType y, templateType z);

		/// \brief Write a vector, using 10 bytes instead of 12.
		/// \details Loses accuracy to about 3/10ths and only saves 2 bytes, 
		/// so only use if accuracy is not important.
		/// \param[in] x x
		/// \param[in] y y
		/// \param[in] z z
		template <class templateType> // templateType for this function must be a float or double
		void WriteVector(templateType x, templateType y, templateType z);

		/// \brief Write a normalized quaternion in 6 bytes + 4 bits instead of 16 bytes.  Slightly lossy.
		/// \param[in] w w
		/// \param[in] x x
		/// \param[in] y y
		/// \param[in] z z
		template <class templateType> // templateType for this function must be a float or double
		void WriteNormQuat(templateType w, templateType x, templateType y, templateType z);

		/// \brief Write an orthogonal matrix by creating a quaternion, and writing 3 components of the quaternion in 2 bytes each.
		/// \details Use 6 bytes instead of 36
		/// Lossy, although the result is renormalized
		template <class templateType> // templateType for this function must be a float or double
		void WriteOrthMatrix(
			templateType m00, templateType m01, templateType m02,
			templateType m10, templateType m11, templateType m12,
			templateType m20, templateType m21, templateType m22);

		/// \param[in] output The result byte array. It should be larger than @em numberOfBytes.
		/// \param[in] numberOfBytes The number of byte to read
		/// \return true on success false if there is some missing bytes.
		bool Read(char* outByteArray, const unsigned int numberOfBytes);

		/// \brief Read a normalized 3D vector, using (at most) 4 bytes + 3 bits instead of 12-24 bytes.  
		/// \details Will further compress y or z axis aligned vectors.
		/// Accurate to 1/32767.5.
		/// \param[in] x x
		/// \param[in] y y
		/// \param[in] z z
		/// \return true on success, false on failure.
		template <class templateType> // templateType for this function must be a float or double
		bool ReadNormVector(templateType& x, templateType& y, templateType& z);

		/// \brief Read 3 floats or doubles, using 10 bytes, where those float or doubles comprise a vector.
		/// \details Loses accuracy to about 3/10ths and only saves 2 bytes, 
		/// so only use if accuracy is not important.
		/// \param[in] x x
		/// \param[in] y y
		/// \param[in] z z
		/// \return true on success, false on failure.
		template <class templateType> // templateType for this function must be a float or double
		bool ReadVector(templateType& x, templateType& y, templateType& z);

		/// \brief Read a normalized quaternion in 6 bytes + 4 bits instead of 16 bytes.
		/// \param[in] w w
		/// \param[in] x x
		/// \param[in] y y
		/// \param[in] z z
		/// \return true on success, false on failure.
		template <class templateType> // templateType for this function must be a float or double
		bool ReadNormQuat(templateType& w, templateType& x, templateType& y, templateType& z);

		/// \brief Read an orthogonal matrix from a quaternion, reading 3 components of the quaternion in 2 bytes each and extrapolatig the 4th.
		/// \details Use 6 bytes instead of 36
		/// Lossy, although the result is renormalized
		/// \return true on success, false on failure.
		template <class templateType> // templateType for this function must be a float or double
		bool ReadOrthMatrix(
			templateType& m00, templateType& m01, templateType& m02,
			templateType& m10, templateType& m11, templateType& m12,
			templateType& m20, templateType& m21, templateType& m22);

		/// ---- Member function template specialization declarations ----
		inline static bool DoEndianSwap(void) {
#ifndef __BITSTREAM_NATIVE_END
			return IsNetworkOrder() == false;
#else
			return false;
#endif
		}
		inline static bool IsBigEndian(void)
		{
			return IsNetworkOrder();
		}
		inline static bool IsNetworkOrder(void) { bool r = IsNetworkOrderInternal(); return r; }

	private:
		BitSize_t numberOfBitsUsed;

		BitSize_t numberOfBitsAllocated;

		BitSize_t readOffset;

		unsigned char* data;

		/// true if the internal buffer is copy of the data passed to the constructor
		bool copyData;

		/// BitStreams that use less than BITSTREAM_STACK_ALLOCATION_SIZE use the stack, rather than the heap to store data.  It switches over if BITSTREAM_STACK_ALLOCATION_SIZE is exceeded
		unsigned char stackData[BITSTREAM_STACK_ALLOCATION_SIZE];
	}; // class BitStream

	template <class templateType>
	inline bool BitStream::Serialize(bool writeToBitstream, templateType& inOutTemplateVar)
	{
		if (writeToBitstream)
			Write(inOutTemplateVar);
		else
			return Read(inOutTemplateVar);
		return true;
	}

	template <class templateType>
	inline bool BitStream::SerializeDelta(bool writeToBitstream, templateType& inOutCurrentValue, const templateType& lastValue)
	{
		if (writeToBitstream)
			WriteDelta(inOutCurrentValue, lastValue);
		else
			return ReadDelta(inOutCurrentValue);
		return true;
	}

	template <class templateType>
	inline bool BitStream::SerializeDelta(bool writeToBitstream, templateType& inOutCurrentValue)
	{
		if (writeToBitstream)
			WriteDelta(inOutCurrentValue);
		else
			return ReadDelta(inOutCurrentValue);
		return true;
	}

	template <class templateType>
	inline bool BitStream::SerializeCompressed(bool writeToBitstream, templateType& inOutTemplateVar)
	{
		if (writeToBitstream)
			WriteCompressed(inOutTemplateVar);
		else
			return ReadCompressed(inOutTemplateVar);
		return true;
	}

	template <class templateType>
	inline bool BitStream::SerializeCompressedDelta(bool writeToBitstream, templateType& inOutCurrentValue, const templateType& lastValue)
	{
		if (writeToBitstream)
			WriteCompressedDelta(inOutCurrentValue, lastValue);
		else
			return ReadCompressedDelta(inOutCurrentValue);
		return true;
	}

	template <class templateType>
	inline bool BitStream::SerializeCompressedDelta(bool writeToBitstream, templateType& inOutCurrentValue)
	{
		if (writeToBitstream)
			WriteCompressedDelta(inOutCurrentValue);
		else
			return ReadCompressedDelta(inOutCurrentValue);
		return true;
	}

	inline bool BitStream::Serialize(bool writeToBitstream, char* inOutByteArray, const unsigned int numberOfBytes)
	{
		if (writeToBitstream)
			Write(inOutByteArray, numberOfBytes);
		else
			return Read(inOutByteArray, numberOfBytes);
		return true;
	}

	template <class templateType>
	inline bool BitStream::SerializeNormVector(bool writeToBitstream, templateType& x, templateType& y, templateType& z)
	{
		if (writeToBitstream)
			WriteNormVector(x, y, z);
		else
			return ReadNormVector(x, y, z);
		return true;
	}

	template <class templateType>
	inline bool BitStream::SerializeVector(bool writeToBitstream, templateType& x, templateType& y, templateType& z)
	{
		if (writeToBitstream)
			WriteVector(x, y, z);
		else
			return ReadVector(x, y, z);
		return true;
	}

	template <class templateType>
	inline bool BitStream::SerializeNormQuat(bool writeToBitstream, templateType& w, templateType& x, templateType& y, templateType& z)
	{
		if (writeToBitstream)
			WriteNormQuat(w, x, y, z);
		else
			return ReadNormQuat(w, x, y, z);
		return true;
	}

	template <class templateType>
	inline bool BitStream::SerializeOrthMatrix(
		bool writeToBitstream,
		templateType& m00, templateType& m01, templateType& m02,
		templateType& m10, templateType& m11, templateType& m12,
		templateType& m20, templateType& m21, templateType& m22)
	{
		if (writeToBitstream)
			WriteOrthMatrix(m00, m01, m02, m10, m11, m12, m20, m21, m22);
		else
			return ReadOrthMatrix(m00, m01, m02, m10, m11, m12, m20, m21, m22);
		return true;
	}

	inline bool BitStream::SerializeBits(bool writeToBitstream, unsigned char* inOutByteArray, const BitSize_t numberOfBitsToSerialize, const bool rightAlignedBits)
	{
		if (writeToBitstream)
			WriteBits(inOutByteArray, numberOfBitsToSerialize, rightAlignedBits);
		else
			return ReadBits(inOutByteArray, numberOfBitsToSerialize, rightAlignedBits);
		return true;
	}

	template <class templateType>
	inline void BitStream::Write(const templateType& inTemplateVar)
	{
#ifdef _MSC_VER
#pragma warning(disable:4127)   // conditional expression is constant
#endif
		if (sizeof(inTemplateVar) == 1)
			WriteBits((unsigned char*)&inTemplateVar, sizeof(templateType) * 8, true);
		else
		{
#ifndef __BITSTREAM_NATIVE_END
			if (DoEndianSwap())
			{
				unsigned char output[sizeof(templateType)];
				ReverseBytes((unsigned char*)&inTemplateVar, output, sizeof(templateType));
				WriteBits((unsigned char*)output, sizeof(templateType) * 8, true);
			}
			else
#endif
				WriteBits((unsigned char*)&inTemplateVar, sizeof(templateType) * 8, true);
		}
	}

	/// \brief Write a bool to a bitstream.
	/// \param[in] inTemplateVar The value to write
	template <>
	inline void BitStream::Write(const bool& inTemplateVar)
	{
		if (inTemplateVar)
			Write1();
		else
			Write0();
	}

	/// Write a playerID to a bitstream
	/// \param[in] var The value to write
	template <>
	inline void BitStream::Write(const PlayerID& var)
	{
		Write(var.binaryAddress);
		Write(var.port);
	}

	/// Write an networkID to a bitstream
	/// \param[in] var The value to write
	template <>
	inline void BitStream::Write(const NetworkID& var)
	{
		if (NetworkID::IsPeerToPeerMode()) // Use the function rather than directly access the member or DLL users will get an undefined external error
			Write(var.playerId);
		Write(var.localSystemId);
	}

	/// \brief Write any integral type to a bitstream.  
	/// \details If the current value is different from the last value
	/// the current value will be written.  Otherwise, a single bit will be written
	/// \param[in] currentValue The current value to write
	/// \param[in] lastValue The last value to compare against
	template <class templateType>
	inline void BitStream::WriteDelta(const templateType& currentValue, const templateType& lastValue)
	{
		if (currentValue == lastValue)
		{
			Write(false);
		}
		else
		{
			Write(true);
			Write(currentValue);
		}
	}

	/// Write a playerID.  If the current value is different from the last value
	/// the current value will be written.  Otherwise, a single bit will be written
	/// \param[in] currentValue The current value to write
	/// \param[in] lastValue The last value to compare against
	template <>
	inline void BitStream::WriteDelta(const PlayerID& currentValue, const PlayerID& lastValue)
	{
		if (currentValue == lastValue)
		{
			Write(false);
		}
		else
		{
			Write(true);
			Write(currentValue.binaryAddress);
			Write(currentValue.port);
		}
	}

	/// Write a playerID.  If the current value is different from the last value
	/// the current value will be written.  Otherwise, a single bit will be written
	/// \param[in] currentValue The current value to write
	/// \param[in] lastValue The last value to compare against
	template <>
	inline void BitStream::WriteDelta(const NetworkID& currentValue, const NetworkID& lastValue)
	{
		if (currentValue == lastValue)
		{
			Write(false);
		}
		else
		{
			Write(true);
			if (NetworkID::IsPeerToPeerMode()) // Use the function rather than directly access the member or DLL users will get an undefined external error
				Write(currentValue.playerId);
			Write(currentValue.localSystemId);
		}
	}

	/// \brief Write a bool delta. Same thing as just calling Write
	/// \param[in] currentValue The current value to write
	/// \param[in] lastValue The last value to compare against
	template <>
	inline void BitStream::WriteDelta(const bool& currentValue, const bool& lastValue)
	{
		(void)lastValue;

		Write(currentValue);
	}

	/// \brief WriteDelta when you don't know what the last value is, or there is no last value.
	/// \param[in] currentValue The current value to write
	template <class templateType>
	inline void BitStream::WriteDelta(const templateType& currentValue)
	{
		Write(true);
		Write(currentValue);
	}

	/// \brief Write any integral type to a bitstream.  
	/// \details Undefine __BITSTREAM_NATIVE_END if you need endian swapping.
	/// For floating point, this is lossy, using 2 bytes for a float and 4 for a double.  The range must be between -1 and +1.
	/// For non-floating point, this is lossless, but only has benefit if you use less than half the bits of the type
	/// If you are not using __BITSTREAM_NATIVE_END the opposite is true for types larger than 1 byte
	/// \param[in] inTemplateVar The value to write
	template <class templateType>
	inline void BitStream::WriteCompressed(const templateType& inTemplateVar)
	{
#ifdef _MSC_VER
#pragma warning(disable:4127)   // conditional expression is constant
#endif
		if (sizeof(inTemplateVar) == 1)
			WriteCompressed((unsigned char*)&inTemplateVar, sizeof(templateType) * 8, true);
		else
		{
#ifndef __BITSTREAM_NATIVE_END
			if (DoEndianSwap())
			{
				unsigned char output[sizeof(templateType)];
				ReverseBytes((unsigned char*)&inTemplateVar, output, sizeof(templateType));
				WriteCompressed((unsigned char*)output, sizeof(templateType) * 8, true);
			}
			else
#endif
				WriteCompressed((unsigned char*)&inTemplateVar, sizeof(templateType) * 8, true);
		}
	}

	template <>
	inline void BitStream::WriteCompressed(const PlayerID& var)
	{
		Write(var);
	}

	template <>
	inline void BitStream::WriteCompressed(const NetworkID& var)
	{
		Write(var);
	}

	template <>
	inline void BitStream::WriteCompressed(const bool& inTemplateVar)
	{
		Write(inTemplateVar);
	}

	/// For values between -1 and 1
	template <>
	inline void BitStream::WriteCompressed(const float& inTemplateVar)
	{
		RakAssert(inTemplateVar > -1.01f && inTemplateVar < 1.01f);
		float varCopy = inTemplateVar;
		if (varCopy < -1.0f)
			varCopy = -1.0f;
		if (varCopy > 1.0f)
			varCopy = 1.0f;
		Write((unsigned short)((varCopy + 1.0f) * 32767.5f));
	}

	/// For values between -1 and 1
	template <>
	inline void BitStream::WriteCompressed(const double& inTemplateVar)
	{
		RakAssert(inTemplateVar > -1.01 && inTemplateVar < 1.01);
		double varCopy = inTemplateVar;
		if (varCopy < -1.0f)
			varCopy = -1.0f;
		if (varCopy > 1.0f)
			varCopy = 1.0f;
		Write((RakUInt32)((varCopy + 1.0) * 2147483648.0));
	}

	/// \brief Write any integral type to a bitstream.  
	/// \details If the current value is different from the last value
	/// the current value will be written.  Otherwise, a single bit will be written
	/// For floating point, this is lossy, using 2 bytes for a float and 4 for a double.  The range must be between -1 and +1.
	/// For non-floating point, this is lossless, but only has benefit if you use less than half the bits of the type
	/// If you are not using __BITSTREAM_NATIVE_END the opposite is true for types larger than 1 byte
	/// \param[in] currentValue The current value to write
	/// \param[in] lastValue The last value to compare against
	template <class templateType>
	inline void BitStream::WriteCompressedDelta(const templateType& currentValue, const templateType& lastValue)
	{
		if (currentValue == lastValue)
		{
			Write(false);
		}
		else
		{
			Write(true);
			WriteCompressed(currentValue);
		}
	}

	/// \brief Write a bool delta.  Same thing as just calling Write
	/// \param[in] currentValue The current value to write
	/// \param[in] lastValue The last value to compare against
	template <>
	inline void BitStream::WriteCompressedDelta(const bool& currentValue, const bool& lastValue)
	{
		(void)lastValue;

		Write(currentValue);
	}

	/// \brief Save as WriteCompressedDelta(const templateType &currentValue, const templateType &lastValue) 
	/// when we have an unknown second parameter
	template <class templateType>
	inline void BitStream::WriteCompressedDelta(const templateType& currentValue)
	{
		Write(true);
		WriteCompressed(currentValue);
	}

	/// \brief Save as WriteCompressedDelta(bool currentValue, const templateType &lastValue) 
	/// when we have an unknown second bool
	template <>
	inline void BitStream::WriteCompressedDelta(const bool& currentValue)
	{
		Write(currentValue);
	}

	/// \brief Read any integral type from a bitstream.  Define __BITSTREAM_NATIVE_END if you need endian swapping.
	/// \param[in] outTemplateVar The value to read
	template <class templateType>
	inline bool BitStream::Read(templateType& outTemplateVar)
	{
#ifdef _MSC_VER
#pragma warning(disable:4127)   // conditional expression is constant
#endif
		if (sizeof(outTemplateVar) == 1)
			return ReadBits((unsigned char*)&outTemplateVar, sizeof(templateType) * 8, true);
		else
		{
#ifndef __BITSTREAM_NATIVE_END
			if (DoEndianSwap())
			{
				unsigned char output[sizeof(templateType)];
				if (ReadBits((unsigned char*)output, sizeof(templateType) * 8, true))
				{
					ReverseBytes(output, (unsigned char*)&outTemplateVar, sizeof(templateType));
					return true;
				}
				return false;
			}
			else
#endif
				return ReadBits((unsigned char*)&outTemplateVar, sizeof(templateType) * 8, true);
		}
	}

	/// \brief Read a bool from a bitstream.
	/// \param[in] outTemplateVar The value to read
	template <>
	inline bool BitStream::Read(bool& outTemplateVar)
	{
		if (GetNumberOfUnreadBits() == 0)
			return false;

		if (data[readOffset >> 3] & (0x80 >> (readOffset & 7)))   // Is it faster to just write it out here?
			outTemplateVar = true;
		else
			outTemplateVar = false;

		// Has to be on a different line for Mac
		readOffset++;

		return true;
	}

	/// Read a playerID from a bitstream
	/// \param[in] var The value to read
	template <>
	inline bool BitStream::Read(PlayerID& var)
	{
		Read(var.binaryAddress);
		return Read(var.port);
	}

	/// Read an networkID from a bitstream
	/// \param[in] var The value to read
	template <>
	inline bool BitStream::Read(NetworkID& var)
	{
		if (NetworkID::IsPeerToPeerMode()) // Use the function rather than directly access the member or DLL users will get an undefined external error
			Read(var.playerId);
		return Read(var.localSystemId);
	}

	/// \brief Read any integral type from a bitstream.  
	/// \details If the written value differed from the value compared against in the write function,
	/// var will be updated.  Otherwise it will retain the current value.
	/// ReadDelta is only valid from a previous call to WriteDelta
	/// \param[in] outTemplateVar The value to read
	template <class templateType>
	inline bool BitStream::ReadDelta(templateType& outTemplateVar)
	{
		bool dataWritten;
		bool success;
		success = Read(dataWritten);
		if (dataWritten)
			success = Read(outTemplateVar);
		return success;
	}

	/// \brief Read a bool from a bitstream.
	/// \param[in] outTemplateVar The value to read
	template <>
	inline bool BitStream::ReadDelta(bool& outTemplateVar)
	{
		return Read(outTemplateVar);
	}

	/// \brief Read any integral type from a bitstream.  
	/// \details Undefine __BITSTREAM_NATIVE_END if you need endian swapping.
	/// For floating point, this is lossy, using 2 bytes for a float and 4 for a double.  The range must be between -1 and +1.
	/// For non-floating point, this is lossless, but only has benefit if you use less than half the bits of the type
	/// If you are not using __BITSTREAM_NATIVE_END the opposite is true for types larger than 1 byte
	/// \param[in] outTemplateVar The value to read
	template <class templateType>
	inline bool BitStream::ReadCompressed(templateType& outTemplateVar)
	{
#ifdef _MSC_VER
#pragma warning(disable:4127)   // conditional expression is constant
#endif
		if (sizeof(outTemplateVar) == 1)
			return ReadCompressed((unsigned char*)&outTemplateVar, sizeof(templateType) * 8, true);
		else
		{
#ifndef __BITSTREAM_NATIVE_END
			if (DoEndianSwap())
			{
				unsigned char output[sizeof(templateType)];
				if (ReadCompressed((unsigned char*)output, sizeof(templateType) * 8, true))
				{
					ReverseBytes(output, (unsigned char*)&outTemplateVar, sizeof(templateType));
					return true;
				}
				return false;
			}
			else
#endif
				return ReadCompressed((unsigned char*)&outTemplateVar, sizeof(templateType) * 8, true);
		}
	}

	template <>
	inline bool BitStream::ReadCompressed(PlayerID& var)
	{
		return Read(var);
	}

	template <>
	inline bool BitStream::ReadCompressed(NetworkID& var)
	{
		return Read(var);
	}

	template <>
	inline bool BitStream::ReadCompressed(bool& outTemplateVar)
	{
		return Read(outTemplateVar);
	}

	/// For values between -1 and 1
	template <>
	inline bool BitStream::ReadCompressed(float& outTemplateVar)
	{
		unsigned short compressedFloat;
		if (Read(compressedFloat))
		{
			outTemplateVar = ((float)compressedFloat / 32767.5f - 1.0f);
			return true;
		}
		return false;
	}

	/// For values between -1 and 1
	template <>
	inline bool BitStream::ReadCompressed(double& outTemplateVar)
	{
		RakUInt32 compressedFloat;
		if (Read(compressedFloat))
		{
			outTemplateVar = ((double)compressedFloat / 2147483648.0 - 1.0);
			return true;
		}
		return false;
	}

	/// \brief Read any integral type from a bitstream.  
	/// \details If the written value differed from the value compared against in the write function,
	/// var will be updated.  Otherwise it will retain the current value.
	/// the current value will be updated.
	/// For floating point, this is lossy, using 2 bytes for a float and 4 for a double.  The range must be between -1 and +1.
	/// For non-floating point, this is lossless, but only has benefit if you use less than half the bits of the type
	/// If you are not using __BITSTREAM_NATIVE_END the opposite is true for types larger than 1 byte
	/// ReadCompressedDelta is only valid from a previous call to WriteDelta
	/// \param[in] outTemplateVar The value to read
	template <class templateType>
	inline bool BitStream::ReadCompressedDelta(templateType& outTemplateVar)
	{
		bool dataWritten;
		bool success;
		success = Read(dataWritten);
		if (dataWritten)
			success = ReadCompressed(outTemplateVar);
		return success;
	}

	/// \brief Read a bool from a bitstream.
	/// \param[in] outTemplateVar The value to read
	template <>
	inline bool BitStream::ReadCompressedDelta(bool& outTemplateVar)
	{
		return Read(outTemplateVar);
	}

	template <class templateType> // templateType for this function must be a float or double
	void BitStream::WriteNormVector(templateType x, templateType y, templateType z)
	{
#ifdef _DEBUG
		RakAssert(x <= 1.01 && y <= 1.01 && z <= 1.01 && x >= -1.01 && y >= -1.01 && z >= -1.01);
#endif

		WriteFloat16((float)x, -1.0f, 1.0f);
		WriteFloat16((float)y, -1.0f, 1.0f);
		WriteFloat16((float)z, -1.0f, 1.0f);
	}

	template <class templateType> // templateType for this function must be a float or double
	void BitStream::WriteVector(templateType x, templateType y, templateType z)
	{
		templateType magnitude = sqrt(x * x + y * y + z * z);
		Write((float)magnitude);
		if (magnitude > 0.00001f)
		{
			WriteCompressed((float)(x / magnitude));
			WriteCompressed((float)(y / magnitude));
			WriteCompressed((float)(z / magnitude));
			//	Write((unsigned short)((x/magnitude+1.0f)*32767.5f));
			//	Write((unsigned short)((y/magnitude+1.0f)*32767.5f));
			//	Write((unsigned short)((z/magnitude+1.0f)*32767.5f));
		}
	}

	template <class templateType> // templateType for this function must be a float or double
	void BitStream::WriteNormQuat(templateType w, templateType x, templateType y, templateType z)
	{
		Write((bool)(w < 0.0));
		Write((bool)(x < 0.0));
		Write((bool)(y < 0.0));
		Write((bool)(z < 0.0));
		Write((unsigned short)(fabs(x) * 65535.0));
		Write((unsigned short)(fabs(y) * 65535.0));
		Write((unsigned short)(fabs(z) * 65535.0));
		// Leave out w and calculate it on the target
	}

	template <class templateType> // templateType for this function must be a float or double
	void BitStream::WriteOrthMatrix(
		templateType m00, templateType m01, templateType m02,
		templateType m10, templateType m11, templateType m12,
		templateType m20, templateType m21, templateType m22)
	{
		double qw;
		double qx;
		double qy;
		double qz;

		// Convert matrix to quat
		// http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
		float sum;
		sum = 1 + m00 + m11 + m22;
		if (sum < 0.0f) sum = 0.0f;
		qw = sqrt(sum) / 2;
		sum = 1 + m00 - m11 - m22;
		if (sum < 0.0f) sum = 0.0f;
		qx = sqrt(sum) / 2;
		sum = 1 - m00 + m11 - m22;
		if (sum < 0.0f) sum = 0.0f;
		qy = sqrt(sum) / 2;
		sum = 1 - m00 - m11 + m22;
		if (sum < 0.0f) sum = 0.0f;
		qz = sqrt(sum) / 2;
		if (qw < 0.0) qw = 0.0;
		if (qx < 0.0) qx = 0.0;
		if (qy < 0.0) qy = 0.0;
		if (qz < 0.0) qz = 0.0;
		qx = _copysign((double)qx, (double)(m21 - m12));
		qy = _copysign((double)qy, (double)(m02 - m20));
		qz = _copysign((double)qz, (double)(m10 - m01));

		WriteNormQuat(qw, qx, qy, qz);
	}

	template <class templateType> // templateType for this function must be a float or double
	bool BitStream::ReadNormVector(templateType& x, templateType& y, templateType& z)
	{
		float xIn, yIn, zIn;
		ReadFloat16(xIn, -1.0f, 1.0f);
		ReadFloat16(yIn, -1.0f, 1.0f);
		ReadFloat16(zIn, -1.0f, 1.0f);
		x = xIn;
		y = yIn;
		z = zIn;
		return true;
	}

	template <class templateType> // templateType for this function must be a float or double
	bool BitStream::ReadVector(templateType& x, templateType& y, templateType& z)
	{
		float magnitude;
		//unsigned short sx,sy,sz;
		if (!Read(magnitude))
			return false;
		if (magnitude > 0.00001f)
		{
			//	Read(sx);
			//	Read(sy);
			//	if (!Read(sz))
			//		return false;
			//	x=((float)sx / 32767.5f - 1.0f) * magnitude;
			//	y=((float)sy / 32767.5f - 1.0f) * magnitude;
			//	z=((float)sz / 32767.5f - 1.0f) * magnitude;
			float cx = 0.0f, cy = 0.0f, cz = 0.0f;
			ReadCompressed(cx);
			ReadCompressed(cy);
			if (!ReadCompressed(cz))
				return false;
			x = cx;
			y = cy;
			z = cz;
			x *= magnitude;
			y *= magnitude;
			z *= magnitude;
		}
		else
		{
			x = 0.0;
			y = 0.0;
			z = 0.0;
		}
		return true;
	}

	template <class templateType> // templateType for this function must be a float or double
	bool BitStream::ReadNormQuat(templateType& w, templateType& x, templateType& y, templateType& z)
	{
		bool cwNeg = false, cxNeg = false, cyNeg = false, czNeg = false;
		unsigned short cx, cy, cz;
		Read(cwNeg);
		Read(cxNeg);
		Read(cyNeg);
		Read(czNeg);
		Read(cx);
		Read(cy);
		if (!Read(cz))
			return false;

		// Calculate w from x,y,z
		x = (templateType)(cx / 65535.0);
		y = (templateType)(cy / 65535.0);
		z = (templateType)(cz / 65535.0);
		if (cxNeg) x = -x;
		if (cyNeg) y = -y;
		if (czNeg) z = -z;
		float difference = 1.0f - x * x - y * y - z * z;
		if (difference < 0.0f)
			difference = 0.0f;
		w = (templateType)(sqrt(difference));
		if (cwNeg)
			w = -w;

		return true;
	}

	template <class templateType> // templateType for this function must be a float or double
	bool BitStream::ReadOrthMatrix(
		templateType& m00, templateType& m01, templateType& m02,
		templateType& m10, templateType& m11, templateType& m12,
		templateType& m20, templateType& m21, templateType& m22)
	{
		float qw, qx, qy, qz;
		if (!ReadNormQuat(qw, qx, qy, qz))
			return false;

		// Quat to orthogonal rotation matrix
		// http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/index.htm
		double sqw = (double)qw * (double)qw;
		double sqx = (double)qx * (double)qx;
		double sqy = (double)qy * (double)qy;
		double sqz = (double)qz * (double)qz;
		m00 = (templateType)(sqx - sqy - sqz + sqw); // since sqw + sqx + sqy + sqz =1
		m11 = (templateType)(-sqx + sqy - sqz + sqw);
		m22 = (templateType)(-sqx - sqy + sqz + sqw);

		double tmp1 = (double)qx * (double)qy;
		double tmp2 = (double)qz * (double)qw;
		m10 = (templateType)(2.0 * (tmp1 + tmp2));
		m01 = (templateType)(2.0 * (tmp1 - tmp2));

		tmp1 = (double)qx * (double)qz;
		tmp2 = (double)qy * (double)qw;
		m20 = (templateType)(2.0 * (tmp1 - tmp2));
		m02 = (templateType)(2.0 * (tmp1 + tmp2));
		tmp1 = (double)qy * (double)qz;
		tmp2 = (double)qx * (double)qw;
		m21 = (templateType)(2.0 * (tmp1 + tmp2));
		m12 = (templateType)(2.0 * (tmp1 - tmp2));

		return true;
	}













}; // namespace RakNet
#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
