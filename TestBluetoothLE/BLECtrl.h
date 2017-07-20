/*
   Bluetooth LE�̑���N���X
*/

#pragma once

#include <setupapi.h> 
#include <BthLEDef.h>
#include <vector>

class BLEService
{
public:
	CString path;				// �T�[�r�X�p�X
	HANDLE handle;				// �n���h��
	PBTH_LE_GATT_SERVICE data;	// �T�[�r�X�̏��
};

class BLEChracteristic
{
public:
	BLEService parent;			// �e�̃T�[�r�X
	PBTH_LE_GATT_CHARACTERISTIC data;	// �L�����N�^���X�e�B�b�N�̏��
};

class CBLECtrl
{
public:
	CBLECtrl();
	~CBLECtrl();

	bool OpenDevice();
	bool CloseDevice();

	bool SearchService(const BTH_LE_UUID service_uuid, BLEService& service);
	bool SearchChracteritic(const BLEService service, const BTH_LE_UUID characteristic_uuid, BLEChracteristic& charcteristic);
	bool ReadChracteritic(const BLEChracteristic charcteristic, std::vector<UCHAR>& data);
	bool WriteChracteritic(const BLEChracteristic charcteristic, const std::vector<UCHAR>& data, const bool responce);
	bool WriteNotify(const BLEChracteristic charcteristic);

	bool GetDescriptors(BLEChracteristic charcteristic);

private:
	bool GetDeviceInterfacePath(CString& path);
	bool GetServicePath(BTH_LE_UUID service_uuid, CString& path);
	GUID uuid2guid(BTH_LE_UUID uuid);
	bool CheckUUID(BTH_LE_UUID uuid1, BTH_LE_UUID uuid2);

private:
	HANDLE hDevice_;
	PBTH_LE_GATT_SERVICE services_;
	USHORT services_size_;
	PBTH_LE_GATT_CHARACTERISTIC characteristics_;
	USHORT characteristics_size_;
	PBTH_LE_GATT_DESCRIPTOR descriptors_;
	USHORT descriptor_size_;
};