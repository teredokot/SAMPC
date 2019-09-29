//----------------------------------------------------------
//
//   SA:MP Multiplayer Modification For GTA:SA
//   Copyright 2004-2006 SA:MP team
//
//----------------------------------------------------------

#include "signer.h"
#include "stdio.h"
#include "memory.h"

//----------------------------------------------

using namespace big;

//----------------------------------------------

void CSigner::GenerateKey(char* szFilename)
{
	m_rsaCrypt.generateKeys();
	m_rsaCrypt.getPublicKey(m_e,m_n);
	m_rsaCrypt.getPrivateKey(m_p,m_q);
	if (szFilename) 
	{
		FILE *f = fopen(szFilename, "wb");
		
		// Write the bit size just incase
		int iBitSize = sizeof(SIGNER_RSA_BIT_SIZE);
		fwrite(&iBitSize, sizeof(iBitSize), 1, f);
		// Write the public key
		fwrite(&m_e, sizeof(m_e), 1, f);
		fwrite(m_n, sizeof(m_n), 1, f);
		// Write the private key
		fwrite(m_p, sizeof(m_p), 1, f);
		fwrite(m_q, sizeof(m_q), 1, f);

		fclose(f);
	}
}

//----------------------------------------------

bool CSigner::ReadKeyForSigning(char* szFilename)
{
	FILE *f = fopen(szFilename, "rb");

	// Read and verify the bit size
	int iBitSize;
	fread(&iBitSize, sizeof(iBitSize), 1, f);
	if (iBitSize != sizeof(SIGNER_RSA_BIT_SIZE))
	{
		fclose(f);
		return false;
	}

	// Read the public key
	fread(&m_e, sizeof(m_e), 1, f);
	fread(m_n, sizeof(m_n), 1, f);

	// Read the private key
	fread(m_p, sizeof(m_p), 1, f);
	fread(m_q, sizeof(m_q), 1, f);

	return true;
}

//----------------------------------------------

unsigned char* CSigner::GetPublicKey()
{
	memcpy(m_pbPubKeyBuffer, &m_e, sizeof(m_e));
	memcpy(m_pbPubKeyBuffer+sizeof(m_e), m_n, sizeof(m_n));
	return m_pbPubKeyBuffer;
}

//----------------------------------------------

void CSigner::SetPublicKey(unsigned char* pbKey)
{
	memcpy(&m_e, pbKey, sizeof(m_e));
	memcpy(m_n, pbKey+sizeof(m_e), sizeof(m_n));
}

//----------------------------------------------

void CSigner::SetSignature(unsigned char* pbSignature)
{
	memcpy(m_pbSignBuffer, pbSignature, sizeof(m_pbSignBuffer));
}

//----------------------------------------------

void CSigner::SignHash(unsigned char* pbSHA1Hash)
{
	SignData(pbSHA1Hash, SIGNER_SHA1_SIZE);
}

//----------------------------------------------

bool CSigner::VerifyHash(unsigned char* pbSHA1Hash)
{
	unsigned char message[sizeof(SIGNER_RSA_BIT_SIZE)];
	
	// Decrypt it
	DecryptSignBuffer(message);

	// Verify it against the hash provided
	unsigned int* pdwHash = reinterpret_cast<unsigned int*>(message);
	unsigned int* pdwHashGiven = reinterpret_cast<unsigned int*>(pbSHA1Hash);
	int iCount = SIGNER_SHA1_SIZE/sizeof(int);
	for(int i=0; i<iCount; i++) 
	{
		if (pdwHash[i] != pdwHashGiven[i])
			return false;
	}

	// Verification suceeded.
	return true;
}

//----------------------------------------------

void CSigner::SignData(unsigned char* pbData, const unsigned int dwLength)
{
	SIGNER_RSA_BIT_SIZE message, signature;
	
	// Reload the keys
	m_rsaCrypt.reset();
	m_rsaCrypt.setPublicKey(m_e, m_n);
	m_rsaCrypt.setPrivateKey(m_p, m_q);

	// Copy the hash into the message buffer and zero pad it
#ifdef _DEBUG
	if (sizeof(message) < dwLength)
		return;
#endif
	memcpy(message, pbData, dwLength);
	if (dwLength < sizeof(message))
		memset(reinterpret_cast<char*>(message)+dwLength, 0, sizeof(message)-dwLength);

	// Encrypt it to get our signature -- we use the decrypt function here because
	// we're doing signature signing and not encryption. Don't let the name fool you.
	m_rsaCrypt.decrypt(message, signature);

	// Store our signature in the signature buffer
	memcpy(m_pbSignBuffer, &signature, sizeof(m_pbSignBuffer));
}

//----------------------------------------------

void CSigner::DecryptSignBuffer(unsigned char* pbDecryptBuffer)
{
	SIGNER_RSA_BIT_SIZE message, signature;

	// Reload the keys (only need the public key now)
	m_rsaCrypt.reset();
	m_rsaCrypt.setPublicKey(m_e, m_n);

	// Decrypt the signature to get some hash value
	// Again, note that we are doing signature verification and not encryption.
	memcpy(signature, m_pbSignBuffer, sizeof(m_pbSignBuffer));
	m_rsaCrypt.encrypt(signature, message);

	// Copy the decrypted message into the buffer
	memcpy(pbDecryptBuffer, message, sizeof(message));
	
}

//----------------------------------------------