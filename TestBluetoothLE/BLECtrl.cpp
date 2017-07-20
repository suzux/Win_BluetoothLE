/*
	Bluetooth LEの操作クラス
	- Visual Studio 2013以降
	- Windows8.1以降
	- ペアリング済のBLEデバイスとしか通信できない（WindowsAPIの仕様）
	- デバイスマネージャーのBluetoothドングルに「Microsoft Bluetooth LE Enumerator」がある場合のみ動作
*/
#include "stdafx.h"
#include "BLECtrl.h"
#include <uuids.h>
#include <devguid.h>
#include <regstr.h>
#include <BluetoothLEapis.h>
#include <BluetoothAPIs.h>
#include <setupapi.h>
#include <devguid.h>
#include <vector>

#define INITGUID
#include <devpkey.h>

#pragma comment(lib, "setupapi")
#pragma comment(lib, "BluetoothAPIs.lib")
#pragma comment(lib, "SetupAPI")

CBLECtrl::CBLECtrl()
{
}

CBLECtrl::~CBLECtrl()
{
}

/*
* デバイスインターフェースを開く
*/
bool CBLECtrl::OpenDevice()
{
	CString path;
	bool result = GetDeviceInterfacePath(path);
	if (!result)
	{
		return false;
	}

	hDevice_ = CreateFile(path, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	return true;
}

/*
* デバイスインターフェースを閉じる
*/
bool CBLECtrl::CloseDevice()
{
	if (hDevice_ == NULL)
	{
		return false;
	}

	CloseHandle(hDevice_);

	if (GetLastError() != NO_ERROR && GetLastError() != ERROR_NO_MORE_ITEMS)
	{
		return false;
	}
	return true;
}

/*
* 指定UUIDのサービスを探す
*/
bool CBLECtrl::SearchService(const BTH_LE_UUID service_uuid, BLEService& service)
{
	if (hDevice_ == NULL)
	{
		return false;
	}

	// サービス数を取得
	USHORT serviceBufferCount;
	HRESULT hr = BluetoothGATTGetServices(hDevice_, 0, NULL, &serviceBufferCount, BLUETOOTH_GATT_FLAG_NONE);
	if (HRESULT_FROM_WIN32(ERROR_MORE_DATA) != hr)
	{
		printf("BluetoothGATTGetServices - バッファ %d\n", serviceBufferCount);
	}

	// 取得するサービスの領域確保
	services_ = (PBTH_LE_GATT_SERVICE)malloc(sizeof(BTH_LE_GATT_SERVICE) * serviceBufferCount);
	if (services_ == NULL)
	{
		printf("pServiceBuffer out of memory\n");
	}
	else
	{
		RtlZeroMemory(services_, sizeof(BTH_LE_GATT_SERVICE) * serviceBufferCount);
	}

	// サービス一覧取得
	if (BluetoothGATTGetServices(hDevice_, serviceBufferCount, services_, &services_size_, BLUETOOTH_GATT_FLAG_NONE) != S_OK)
	{
		printf("BluetoothGATTGetServices - バッファ %d\n", serviceBufferCount);
		return false;
	}

	if (services_ == NULL)
	{
		return false;
	}

	// 指定UUIDのサービスを探す
	for (int n = 0; n < services_size_; n++)
	{
		if (!CheckUUID(service_uuid, services_[n].ServiceUuid))
		{
			continue;
		}

		// 指定サービスが見つかった
		service.data = &services_[n];
			
		// 指定サービスのパスを取得
		bool result = GetServicePath(service.data->ServiceUuid, service.path);
		if (!result)
		{
			continue;
		}

		// 指定サービスオープン
		service.handle = CreateFile(service.path, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (service.handle == INVALID_HANDLE_VALUE)
		{
			continue;
		}

		return true;
	}

	return false;
}


/*
* 指定サービスの中から指定UUIDのキャラクタリスティックを探す
*/
bool CBLECtrl::SearchChracteritic(const BLEService service, const BTH_LE_UUID characteristic_uuid, BLEChracteristic& charcteristic)
{
	if (hDevice_ == NULL)
	{
		return false;
	}

	if (service.data == NULL)
	{
		return false;
	}

	// 指定サービスから取得できるキャラクタリスティック数を取得
	HRESULT hr = BluetoothGATTGetCharacteristics(hDevice_, service.data, 0, NULL, &characteristics_size_, BLUETOOTH_GATT_FLAG_NONE);
	if (hr != HRESULT_FROM_WIN32(ERROR_MORE_DATA))
	{
		printf("BluetoothGATTGetCharacteristics - Buffer Size %d\n", hr);
	}

	characteristics_ = NULL;
	if (characteristics_size_ <= 0)
	{
		return false;
	}

	// 取得可能数分の領域確保
	characteristics_ = (PBTH_LE_GATT_CHARACTERISTIC)malloc(characteristics_size_ * sizeof(BTH_LE_GATT_CHARACTERISTIC));
	if (characteristics_ == NULL)
	{
		printf("pCharBuffer out of memory\n");
	}
	else
	{
		RtlZeroMemory(characteristics_, characteristics_size_ * sizeof(BTH_LE_GATT_CHARACTERISTIC));
	}

	// 指定サービスからキャラクタリスティックの一覧取得
	USHORT numChars;
	if (BluetoothGATTGetCharacteristics(hDevice_, service.data, characteristics_size_, characteristics_, &numChars, BLUETOOTH_GATT_FLAG_NONE) != S_OK)
	{
		printf("BluetoothGATTGetCharacteristics - Actual Data %d\n", hr);
	}

	if (numChars != characteristics_size_)
	{
		printf("buffer size and buffer size actual size mismatch\n");
	}

	int index = -1;
	for (int n = 0; n < characteristics_size_; n++)
	{
		// 指定UUIDのキャラクタリスティックを探す
		if (CheckUUID(characteristics_[n].CharacteristicUuid, characteristic_uuid))
		{
			charcteristic.data = &characteristics_[n];
			charcteristic.parent = service;
			return true;
		}
	}
	return false;
}

/*
* キャラクタリスティックの読み込み
*/
bool CBLECtrl::ReadChracteritic(const BLEChracteristic charcteristic, std::vector<UCHAR>& data)
{
	if (!charcteristic.data->IsReadable)
	{
		return false;
	}

	USHORT charValueDataSize;
	HRESULT hr = BluetoothGATTGetCharacteristicValue(charcteristic.parent.handle,charcteristic.data,0,NULL,&charValueDataSize,BLUETOOTH_GATT_FLAG_NONE);

	if (HRESULT_FROM_WIN32(ERROR_MORE_DATA) != hr)
	{
		// BluetoothGATTGetCharacteristicValue - Buffer Size
		return false;
	}

	PBTH_LE_GATT_CHARACTERISTIC_VALUE pCharValueBuffer = (PBTH_LE_GATT_CHARACTERISTIC_VALUE)malloc(charValueDataSize);
	if (pCharValueBuffer == NULL)
	{
		return false;
	}

	RtlZeroMemory(pCharValueBuffer, charValueDataSize);

	hr = BluetoothGATTGetCharacteristicValue(charcteristic.parent.handle,charcteristic.data,(ULONG)charValueDataSize,pCharValueBuffer,NULL,	BLUETOOTH_GATT_FLAG_NONE);
	if (hr != S_OK)
	{
		// BluetoothGATTGetCharacteristicValue - Actual Data
		free(pCharValueBuffer);
		return false;
	}

	for (int iii = 0; iii < (int)pCharValueBuffer->DataSize; iii++)
	{
		data.push_back(pCharValueBuffer->Data[iii]);
	}

	free(pCharValueBuffer);
	pCharValueBuffer = NULL;

	return true;
}


/*
* キャラクタリスティックに書き込み
*/
bool CBLECtrl::WriteChracteritic(const BLEChracteristic charcteristic, const std::vector<UCHAR>& data, const bool responce)
{
	PBTH_LE_GATT_CHARACTERISTIC_VALUE newValue = (PBTH_LE_GATT_CHARACTERISTIC_VALUE)malloc(sizeof(BTH_LE_GATT_CHARACTERISTIC_VALUE) * data.size());
	newValue->DataSize = data.size();
	for (int n = 0; n < static_cast<int>(data.size()); n++)
	{
		newValue->Data[n] = data[n];
	}

	HRESULT hr;
	if (responce)
	{
		hr = BluetoothGATTSetCharacteristicValue(charcteristic.parent.handle, charcteristic.data, newValue, NULL, BLUETOOTH_GATT_FLAG_WRITE_WITHOUT_RESPONSE);
	}
	else
	{
		hr = BluetoothGATTSetCharacteristicValue(charcteristic.parent.handle, charcteristic.data, newValue, NULL, BLUETOOTH_GATT_FLAG_NONE);
	}
	if (hr != S_OK)
	{
		return false;
	}

	return true;
}


/*
* NOtify通知
*/
void HandleBLENotification(BTH_LE_GATT_EVENT_TYPE type, PVOID param, PVOID context)
{
	MessageBox(NULL, NULL, _T("ok"), 1);
}

/*
* Notify通知登録
*/
bool CBLECtrl::WriteNotify(const BLEChracteristic charcteristic)
{
	if (!charcteristic.data->IsNotifiable)
	{
		return false;
	}

	BLUETOOTH_GATT_VALUE_CHANGED_EVENT_REGISTRATION EventParameterIn;
	EventParameterIn.Characteristics[0] = *charcteristic.data;
	EventParameterIn.NumCharacteristics = 1;
	BLUETOOTH_GATT_EVENT_HANDLE EventHandle = NULL;
	HRESULT hr = BluetoothGATTRegisterEvent(charcteristic.parent.handle, CharacteristicValueChangedEvent, (PVOID)&EventParameterIn,
		HandleBLENotification, (PVOID)this, &EventHandle, BLUETOOTH_GATT_FLAG_NONE);
	if (hr != S_OK)
	{
		printf("BluetoothGATTRegisterEvent - Actual Data %d", hr);
		return false;
	}
	return true;
}

/*
* ディスクリプタ取得
* 未実装
*/
bool CBLECtrl::GetDescriptors(BLEChracteristic charcteristic)
{
	HRESULT hr = BluetoothGATTGetDescriptors(hDevice_, charcteristic.data, 0, NULL, &descriptor_size_, BLUETOOTH_GATT_FLAG_NONE);

	if (HRESULT_FROM_WIN32(ERROR_MORE_DATA) != hr)
	{
		// BluetoothGATTGetDescriptors - Buffer Size
		return false;
	}

	if (descriptor_size_ > 0)
	{
		descriptors_ = (PBTH_LE_GATT_DESCRIPTOR)malloc(descriptor_size_* sizeof(BTH_LE_GATT_DESCRIPTOR));

		if (NULL == descriptors_)
		{
			// pDescriptorBuffer out of memory
			return false;
		}
		else
		{
			RtlZeroMemory(descriptors_, descriptor_size_);
		}

		USHORT numDescriptors;
		hr = BluetoothGATTGetDescriptors(hDevice_, charcteristic.data, descriptor_size_, descriptors_, &numDescriptors, BLUETOOTH_GATT_FLAG_NONE);
		if (hr != S_OK)
		{
			// BluetoothGATTGetDescriptors - Actual Data
			return false;
		}

		if (numDescriptors != descriptor_size_)
		{
			// buffer size and buffer size actual size mismatch
			return false;
		}
	}

	return true;
}


/*
 * インターフェースのパス取得
 */
bool CBLECtrl::GetDeviceInterfacePath(CString& path)
{
	HDEVINFO hDI;
	SP_DEVICE_INTERFACE_DATA device_interface_data;
	SP_DEVINFO_DATA deveiceinfo;

	// BLE操作クラス取得
	GUID BluetoothClassGUID = GUID_BLUETOOTHLE_DEVICE_INTERFACE;
	hDI = SetupDiGetClassDevs(&BluetoothClassGUID, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
	if (hDI == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	device_interface_data.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	deveiceinfo.cbSize = sizeof(SP_DEVINFO_DATA);

	SP_DEVINFO_DATA DeviceInfoData;
	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	for (int i = 0; SetupDiEnumDeviceInfo(hDI, i, &DeviceInfoData); i++)
	{
		DWORD DataT;
		LPTSTR buffer = NULL;
		DWORD buffersize = 0;
		while (!SetupDiGetDeviceRegistryProperty(hDI,&DeviceInfoData,SPDRP_FRIENDLYNAME,&DataT,	(PBYTE)buffer,buffersize,&buffersize))
		{
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				// Change the buffer size.
				if (buffer)
				{
					delete(buffer);
				}
				// Double the size to avoid problems on
				// W2k MBCS systems per KB 888609.
				buffer = new wchar_t[buffersize * 2];
			}
			else
			{
				// Insert error handling here.
				break;
			}
		}
		DWORD DataT2;
		LPTSTR buffer2 = NULL;
		DWORD buffersize2 = 0;
		while (!SetupDiGetDeviceRegistryProperty(hDI,&DeviceInfoData,SPDRP_HARDWAREID,&DataT2,(PBYTE)buffer2,buffersize2,&buffersize2))
		{
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				// Change the buffer size.
				if (buffer2) delete(buffer2);
				// Double the size to avoid problems on
				// W2k MBCS systems per KB 888609.
				buffer2 = new wchar_t[buffersize2 * 2];
			}
			else
			{
				// Insert error handling here.
				break;
			}
		}
		if (buffersize > 0)
		{
			//do what you need with the info
			//name is in buffer
			//address is in buffer2
		}
	}

	// BLE対応デバイスのパス取得
	GUID BluetoothInterfaceGUID = GUID_BLUETOOTHLE_DEVICE_INTERFACE;
	for (DWORD i = 0; SetupDiEnumDeviceInterfaces(hDI, NULL, &BluetoothInterfaceGUID, i, &device_interface_data); i++)
	{
		SP_DEVICE_INTERFACE_DETAIL_DATA DeviceInterfaceDetailData;

		DeviceInterfaceDetailData.cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

		DWORD size = 0;

		if (!SetupDiGetDeviceInterfaceDetail(hDI, &device_interface_data, NULL, 0, &size, 0))
		{
			int err = GetLastError();

			if (err == ERROR_NO_MORE_ITEMS)
			{
				break;
			}

			PSP_DEVICE_INTERFACE_DETAIL_DATA pInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)GlobalAlloc(GPTR, size);

			pInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

			if (!SetupDiGetDeviceInterfaceDetail(hDI, &device_interface_data, pInterfaceDetailData, size, &size, &deveiceinfo))
			{
				break;
			}

			path = pInterfaceDetailData->DevicePath;
			GlobalFree(pInterfaceDetailData);
		}
	}

	SetupDiDestroyDeviceInfoList(hDI);
	return true;
}


/*
* サービスのパスを取得する
*/
bool CBLECtrl::GetServicePath(BTH_LE_UUID service_uuid, CString& path)
{
	GUID guid = uuid2guid(service_uuid);
	SP_DEVICE_INTERFACE_DATA device_interface_data = { 0 };
	device_interface_data.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);


	HDEVINFO hDevinfo = SetupDiGetClassDevs(&guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (hDevinfo == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	for (int index=0; index<100; index++)
	{
		BOOL result = SetupDiEnumDeviceInterfaces(hDevinfo, NULL, (LPGUID)&guid, index, &device_interface_data);
		if (!result)
		{
			DWORD last_error = GetLastError();
			if (last_error == ERROR_NO_MORE_ITEMS)
			{
				return false;
			}
			else
			{
				// Error enumerating device interfaces
				continue;
			}
		}

		ULONG required_length = 0;
		result = SetupDiGetDeviceInterfaceDetail(hDevinfo, &device_interface_data, NULL, 0, &required_length, NULL);

		PSP_DEVICE_INTERFACE_DETAIL_DATA detail_data = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(sizeof(PSP_DEVICE_INTERFACE_DETAIL_DATA) * required_length);
		if (detail_data == NULL)
		{
			printf("detail_data out of memory\n");
		}
		else
		{
			RtlZeroMemory(detail_data, sizeof(PSP_DEVICE_INTERFACE_DETAIL_DATA) * required_length);
		}

		detail_data->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		ULONG predicted_length = required_length;
		result = SetupDiGetDeviceInterfaceDetail(hDevinfo, &device_interface_data,
			detail_data, predicted_length, &required_length, NULL);

		if (!result)
		{
			continue;
		}

		path = detail_data->DevicePath;
		return true;
	}
	return false;
}

/*
 * UUIDをGUIDに変換
 */
GUID CBLECtrl::uuid2guid(BTH_LE_UUID uuid)
{
	GUID guid(BTH_LE_ATT_BLUETOOTH_BASE_GUID);
	if (uuid.IsShortUuid)
	{
		guid.Data1 = uuid.Value.ShortUuid;
	}
	else
	{
		guid = uuid.Value.LongUuid;
	}
	return guid;
}

/*
 * 同じUUIDかチェック
 */
bool CBLECtrl::CheckUUID(BTH_LE_UUID uuid1, BTH_LE_UUID uuid2)
{
	GUID ble_uuid1 = uuid2guid(uuid1);
	GUID ble_uuid2 = uuid2guid(uuid2);

	if (::IsEqualGUID(ble_uuid1, ble_uuid2))
	{
		return true;
	}

	return false;
}

