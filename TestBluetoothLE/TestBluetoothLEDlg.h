
// TestBluetoothLEDlg.h : header file
//

#pragma once

#include "BLECtrl.h"

// CTestBluetoothLEDlg dialog
class CTestBluetoothLEDlg : public CDialogEx
{
// Construction
public:
	CTestBluetoothLEDlg(CWnd* pParent = NULL);	// standard constructor
	void SetView(CString title, CString data);
	void SetView(CString title, BOOL data);

// Dialog Data
	enum { IDD = IDD_TESTBLUETOOTHLE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Implementation
protected:
	HICON m_hIcon;
	CBLECtrl ble_;

	BLEService read_service;
	BLEService write_service;
	BLEChracteristic read_characteristic;
	BLEChracteristic write_characteristic;


	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnBnClickedButtonOpen2();
	afx_msg void OnBnClickedButtonOpen3();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButtonDescriptors();
	afx_msg void OnBnClickedButtonOpen5();
};
