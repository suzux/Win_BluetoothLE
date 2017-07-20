
// TestBluetoothLEDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestBluetoothLE.h"
#include "TestBluetoothLEDlg.h"
#include "afxdialogex.h"
#include "WinBase.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About



class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTestBluetoothLEDlg dialog



CTestBluetoothLEDlg::CTestBluetoothLEDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTestBluetoothLEDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestBluetoothLEDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestBluetoothLEDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CTestBluetoothLEDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CTestBluetoothLEDlg::OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_OPEN2, &CTestBluetoothLEDlg::OnBnClickedButtonOpen2)
	ON_BN_CLICKED(IDC_BUTTON_OPEN3, &CTestBluetoothLEDlg::OnBnClickedButtonOpen3)
	ON_BN_CLICKED(IDC_BUTTON1, &CTestBluetoothLEDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON_DESCRIPTORS, &CTestBluetoothLEDlg::OnBnClickedButtonDescriptors)
	ON_BN_CLICKED(IDC_BUTTON_OPEN5, &CTestBluetoothLEDlg::OnBnClickedButtonOpen5)
END_MESSAGE_MAP()


// CTestBluetoothLEDlg message handlers

BOOL CTestBluetoothLEDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CListCtrl* list = static_cast<CListCtrl*>(GetDlgItem(IDC_LIST));
	list->InsertColumn(0, _T("name"), LVCFMT_LEFT, 150);
	list->InsertColumn(1, _T("data"), LVCFMT_LEFT, 400);
	list->SetExtendedStyle(LVS_EX_FULLROWSELECT);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestBluetoothLEDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestBluetoothLEDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestBluetoothLEDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


/*
* ÉäÉXÉgÇ…ï\é¶
*/
void CTestBluetoothLEDlg::SetView(CString title, CString data)
{
	CListCtrl* list = static_cast<CListCtrl*>(GetDlgItem(IDC_LIST));
	list->InsertItem(list->GetItemCount(), title);
	list->SetItemText(list->GetItemCount() - 1, 1, data);
	list->EnsureVisible(list->GetItemCount()-1, TRUE);
}

void CTestBluetoothLEDlg::SetView(CString title, BOOL data)
{
	CString text;
	text.Format(_T("%d"), data);
	SetView(title, text);
}



/*
* Open
*/
void CTestBluetoothLEDlg::OnBnClickedButtonOpen()
{
	if (ble_.OpenDevice())
	{
		SetView(_T("Open"), _T("ê¨å˜"));
	}
	else
	{
		SetView(_T("Open"), _T("é∏îs"));
	}
}

/*
* CloseDevice
*/
void CTestBluetoothLEDlg::OnBnClickedButtonClose()
{
	if (ble_.CloseDevice())
	{
		SetView(_T("Close"), _T("ê¨å˜"));
	}
	else
	{
		SetView(_T("Close"), _T("é∏îs"));
	}
}

/*#define BURUTAGU_SERVICE_UUID _T("2b1da6de-9c29-4d6c-a930-b990ea2f12bb")
#define BURUTAGU_WRITE_CHARACTERLESTIC_UUID _T("1F1E1D1C-1B1A-1918-1716-151413121110")
#define BURUTAGU_READ_CHARACTERLESTIC_UUID _T("2F2E2D2C-2B2A-2928-2726-252423222120")
#define BURUTAGU_NOTIFY_CHARACTERLESTIC_UUID _T("2F2E2D2C-2B2A-2928-2726-252423222120")
*/


#define BURUTAGU_READ_SERVICE_UUID 0x180F
#define BURUTAGU_READ_CHARACTERLESTIC_UUID 0x2A1B

#define BURUTAGU_WRITE_SERVICE_UUID 0x1803
#define BURUTAGU_WRITE_CHARACTERLESTIC_UUID 0x2A06

/*
* Search
*/
void CTestBluetoothLEDlg::OnBnClickedButtonOpen3()
{
	BTH_LE_UUID read_service_uuid;
	BTH_LE_UUID write_service_uuid;
	BTH_LE_UUID read_characteristic_uuid;
	BTH_LE_UUID write_characteristic_uuid;

	read_service_uuid.IsShortUuid = true;
	read_service_uuid.Value.ShortUuid = BURUTAGU_READ_SERVICE_UUID;

	write_service_uuid.IsShortUuid = true;
	write_service_uuid.Value.ShortUuid = BURUTAGU_WRITE_SERVICE_UUID;

	read_characteristic_uuid.IsShortUuid = true;
	read_characteristic_uuid.Value.ShortUuid = BURUTAGU_READ_CHARACTERLESTIC_UUID;

	write_characteristic_uuid.IsShortUuid = true;
	write_characteristic_uuid.Value.ShortUuid = BURUTAGU_WRITE_CHARACTERLESTIC_UUID;

	if (!ble_.SearchService(read_service_uuid, read_service))
	{
		SetView(_T("Read SearchService"), _T("é∏îs"));
		return;
	}
	SetView(_T("Read SearchService"), _T("ê¨å˜"));

	if (!ble_.SearchChracteritic(read_service, read_characteristic_uuid, read_characteristic))
	{
		SetView(_T("Read  SearchChracteritic"), _T("é∏îs"));
		return;
	}
	SetView(_T("Read GetCharacteristics"), _T("ê¨å˜"));

	if (!ble_.SearchService(write_service_uuid, write_service))
	{
		SetView(_T("Write SearchService"), _T("é∏îs"));
		return;
	}
	SetView(_T("Write SearchService"), _T("ê¨å˜"));

	if (!ble_.SearchChracteritic(write_service, write_characteristic_uuid, write_characteristic))
	{
		SetView(_T("Write  SearchChracteritic"), _T("é∏îs"));
		return;
	}

	SetView(_T("Write SearchChracteritic"), _T("ê¨å˜"));
}

/*
* Read
*/
void CTestBluetoothLEDlg::OnBnClickedButtonDescriptors()
{
	std::vector<UCHAR> data;
	if (ble_.ReadChracteritic(read_characteristic, data))
	{
		SetView(_T("Read"), _T("ê¨å˜"));
		CString text;
		for (int n = 0; n < static_cast<int>(data.size()); n++)
		{
			CString temp;
			temp.Format(_T("%2x"), data[n]);
			text += temp;
		}
		SetView(_T("charceristic"), text);
	}
	else
	{
		SetView(_T("Read"), _T("é∏îs"));
	}
}

/*
* Write
*/
void CTestBluetoothLEDlg::OnBnClickedButtonOpen2()
{
	std::vector<UCHAR> data;
	data.push_back(0x01);
	data.push_back(0x02);
	data.push_back(0x03);
	data.push_back(0x04);
	data.push_back(0x05);
	data.push_back(0x06);

	if (ble_.WriteChracteritic(write_characteristic, data, true))
	{
		SetView(_T("WriteChracteritic"), _T("ê¨å˜"));
	}
	else
	{
		SetView(_T("WriteChracteritic"), _T("é∏îs"));
	}
}

/*
* è¡Ç∑
*/
void CTestBluetoothLEDlg::OnBnClickedButton1()
{
	CListCtrl* list = static_cast<CListCtrl*>(GetDlgItem(IDC_LIST));
	list->DeleteAllItems();
}



/*
* Notifyí ímìoò^
*/
void CTestBluetoothLEDlg::OnBnClickedButtonOpen5()
{
	if (ble_.WriteNotify(read_characteristic))
	{
		SetView(_T("WriteNotify"), _T("ê¨å˜"));
	}
	else
	{
		SetView(_T("WriteNotify"), _T("é∏îs"));
	}
}
