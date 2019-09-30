// 
// Version: $Id: helpdialog.h,v 1.2 2006/03/20 17:44:19 kyeman Exp $
//

#pragma once
class CHelpDialog
{
private:

	IDirect3DDevice9 *m_pD3DDevice;

public:
	CHelpDialog(IDirect3DDevice9 *pD3DDevice);
	~CHelpDialog() {};

	void Draw();
};
