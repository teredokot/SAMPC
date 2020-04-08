#ifndef __RANGE_LIST_H
#define __RANGE_LIST_H

#include "DS_OrderedList.h"
#include "BitStream.h"
#include "RakAssert.h"

namespace DataStructures
{
	template <class range_type>
	struct RangeNode
	{
		RangeNode() {}
		~RangeNode() {}
		RangeNode(range_type min, range_type max) {minIndex=min; maxIndex=max;}
		range_type minIndex;
		range_type maxIndex;
	};


	template <class range_type>
	int RangeNodeComp(const range_type &a, const RangeNode<range_type> &b)
	{
		if (a<b.minIndex)
			return -1;
		if (a>b.maxIndex)
			return 1;
		return 0;
	}

	template <class range_type>
	class RAK_DLL_EXPORT RangeList
	{
	public:
		RangeList();
		~RangeList();
		void Insert(range_type index);
		void Clear(void);
		bool IsWithinRange(range_type value) const;
		unsigned Size(void) const;
		unsigned RangeSum(void) const;
		unsigned Serialize(RakNet::BitStream *in, int maxBits, bool clearSerialized);
		bool Deserialize(RakNet::BitStream *out);

		DataStructures::OrderedList<range_type, RangeNode<range_type> , RangeNodeComp<range_type> > ranges;

	private:
		static bool DeserializeSingleRange(RakNet::BitStream *out, range_type& min, range_type& max);
	};

	template <class range_type>
	unsigned RangeList<range_type>::Serialize(RakNet::BitStream *in, int maxBits, bool clearSerialized)
	{	
		RakAssert(ranges.Size() < (unsigned short)-1);
		RakNet::BitStream tempBS;
		int bitsWritten;
		unsigned short countWritten;
		unsigned i;
		countWritten=0;
		bitsWritten=0;
		for (i=0; i < ranges.Size(); i++)
		{
			// #med - review this calculation --- shouldn't this be +8 rather than +1 due to minEqualsMax being 1 byte?
			if ((int)sizeof(unsigned short)*8+bitsWritten+(int)sizeof(range_type)*8*2+1>maxBits)
				break;
			unsigned char minEqualsMax;
			if (ranges[i].minIndex==ranges[i].maxIndex)
				minEqualsMax=1;
			else
				minEqualsMax=0;

			tempBS.Write(minEqualsMax); // Use one byte, instead of one bit, for speed, as this is done a lot
			tempBS.Write(ranges[i].minIndex);
			bitsWritten+=sizeof(range_type)*8+8;
			if (ranges[i].minIndex!=ranges[i].maxIndex)
			{
				tempBS.Write(ranges[i].maxIndex);
				bitsWritten+=sizeof(range_type)*8;
			}
			countWritten++;
		}

		in->AlignWriteToByteBoundary();
		int before=in->GetWriteOffset();
		in->Write(countWritten);
		bitsWritten+=in->GetWriteOffset()-before;
	//	printf("%i ", in->GetNumberOfBitsUsed());
		in->Write(&tempBS, tempBS.GetNumberOfBitsUsed());
	//	printf("%i %i \n", tempBS.GetNumberOfBitsUsed(),in->GetNumberOfBitsUsed());

		if (clearSerialized && countWritten)
		{
			unsigned rangeSize = ranges.Size();
			for (i = 0; i < rangeSize - countWritten; i++) {
				ranges[i] = ranges[i + countWritten];
			}
			ranges.Del(countWritten);
		}

		return bitsWritten;
	}
	template <class range_type>
	bool RangeList<range_type>::Deserialize(RakNet::BitStream *out)
	{
		ranges.Clear();
		unsigned short count;
		out->AlignReadToByteBoundary();
		if (!out->Read(count))
			return false;
		unsigned short i;
		range_type min,max;
		range_type absMin;

		if (count == 0)
			return true;

		if (!DeserializeSingleRange(out, min, max))
			return false;

		ranges.InsertAtEnd(RangeNode<range_type>(min, max));

		for (i=1; i < count; i++)
		{
			absMin=max;
			if (!DeserializeSingleRange(out, min, max))
				return false;
			if (min<=absMin)
				return false;
			ranges.InsertAtEnd(RangeNode<range_type>(min,max));
		}
		return true;
	}

	template <class range_type>
	bool RangeList<range_type>::DeserializeSingleRange(RakNet::BitStream* out, range_type& min, range_type& max)
	{
		unsigned char maxEqualToMin;
		if (!out->Read(maxEqualToMin))
			return false;
		if (!out->Read(min))
			return false;
		if (maxEqualToMin==0)
		{
			if (!out->Read(max))
				return false;
			if (max<=min)
				return false;
		}
		else
			max=min;

		return true;
	}

	template <class range_type>
	RangeList<range_type>::RangeList()
	{
		RangeNodeComp<range_type>(0, RangeNode<range_type>());
	}

	template <class range_type>
	RangeList<range_type>::~RangeList()
	{
		Clear();
	}

	template <class range_type>
	void RangeList<range_type>::Insert(range_type index)
	{
		if (ranges.Size()==0)
		{
			ranges.Insert(index, RangeNode<range_type>(index, index));
			return;
		}

		bool objectExists;
		unsigned insertionIndex=ranges.GetIndexFromKey(index, &objectExists);
		if (objectExists)
			return; // index already covered by a range entry - do not create a duplicated entry

		// index > maxIndex on entire range list
		if (insertionIndex==ranges.Size())
		{
			if (index == ranges[insertionIndex-1].maxIndex+(range_type)1)
				ranges[insertionIndex-1].maxIndex++;
			else if (index > ranges[insertionIndex-1].maxIndex+(range_type)1)
			{
				// Insert at end
				ranges.Insert(index, RangeNode<range_type>(index, index));
			}

			return;
		}

		// verify it's really not within the current range (otherwise objectExists should have been true)
		RakAssert(index < ranges[insertionIndex].minIndex || index > ranges[insertionIndex].maxIndex);

		if (index < ranges[insertionIndex].minIndex-(range_type)1)
		{
			// Insert here
			ranges.InsertAtIndex(RangeNode<range_type>(index, index), insertionIndex);

			return;
		}
		if (index == ranges[insertionIndex].minIndex-(range_type)1)
		{
			// Decrease minIndex and join left
			ranges[insertionIndex].minIndex--;
			if (insertionIndex>0 && ranges[insertionIndex-1].maxIndex+(range_type)1==ranges[insertionIndex].minIndex)
			{
				ranges[insertionIndex-1].maxIndex=ranges[insertionIndex].maxIndex;
				ranges.RemoveAtIndex(insertionIndex);
			}
			return;
		}
		if (index == ranges[insertionIndex].maxIndex+(range_type)1)
		{
			// Increase maxIndex and join right
			ranges[insertionIndex].maxIndex++;
			if (insertionIndex<ranges.Size()-1 && ranges[insertionIndex+(range_type)1].minIndex==ranges[insertionIndex].maxIndex+(range_type)1)
			{
				ranges[insertionIndex+1].minIndex=ranges[insertionIndex].minIndex;
				ranges.RemoveAtIndex(insertionIndex);
			}
		}
	}

	template <class range_type>
	void RangeList<range_type>::Clear(void)
	{
		ranges.Clear();
	}

	template <class range_type>
	bool RangeList<range_type>::IsWithinRange(range_type value) const
	{
		bool objectExists;
		// not interested in the return value
		(void)ranges.GetIndexFromKey(value, &objectExists);
		return objectExists;
	}

	template <class range_type>
	unsigned RangeList<range_type>::Size(void) const
	{
		return ranges.Size();
	}

	template <class range_type>
	unsigned RangeList<range_type>::RangeSum(void) const
	{
		unsigned sum=0,i;
		for (i=0; i < ranges.Size(); i++)
			sum+=ranges[i].maxIndex-ranges[i].minIndex+1;

		return sum;
	}

}

#endif