
// MFCApplication1Dlg.cpp : ���� ����
//

#include "stdafx.h"
#include "MFCApplication1Dlg.h"
#include "afxdialogex.h"
#include "opencv2/opencv.hpp"
#include <cstring>
#include <iostream>
#include <sstream>
#include <cstdlib> //std::system
#include <thread>
#include "SharedImageBuffer.h"
#include <boost/interprocess/sync/scoped_lock.hpp>

#ifndef SHARED_HANDLERS
#include "MFCApplication1.h"
#endif

#include "mythread.h"

using namespace cv;
using namespace std;



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCApplication1Dlg ��ȭ ����



CMFCApplication1Dlg::CMFCApplication1Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MFCAPPLICATION1_DIALOG, pParent), m_radio(1)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCApplication1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICC, m_picc);
	DDX_Radio(pDX, IDC_RADIO1, (int&)m_radio);
}

BEGIN_MESSAGE_MAP(CMFCApplication1Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON3, &CMFCApplication1Dlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCApplication1Dlg::OnBnClickedButton1)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO1, IDC_RADIO3, RadioCtrl)
END_MESSAGE_MAP()


// CMFCApplication1Dlg �޽��� ó����

BOOL CMFCApplication1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
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

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CMFCApplication1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CMFCApplication1Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();

		CDC *pDC;
		CRect rect;

		int count1 = 0;
		int count2 = 0;

		CString str1 = _T("���� : "), str2 = _T("���� : ");

		struct shm_remove
		{
			shm_remove() { shared_memory_object::remove("ODSharedMemory"); }
			~shm_remove() { shared_memory_object::remove("ODSharedMemory"); }
		} remover;

		ti t;

		do {
			t.cam.vc.open("CAR.mp4");
			cout << "Try VideoCapture Open" << endl;
		} while (!t.cam.vc.isOpened());

		t.cam.vc >> t.cam.image;

		//parameter check
		int nchs = t.cam.image.channels();
		std::cout << "\tchannel:" << nchs << std::endl;
		if (nchs != 3 && nchs != 4) {
			t.cam.vc.release();
			getchar();
			return;
		}

		int dsz = t.cam.image.rows * t.cam.image.step;

		//Create a new segment with given name and size
		managed_shared_memory segment(create_only, "ODSharedMemory", dsz * 20);

		//allocation 
		t.od = (odi*)segment.allocate(sizeof(odi));

		assert(t.od != NULL);

		//set image size
		t.od->dsz = dsz;
		t.od->iHeight = t.cam.image.size().height;
		t.od->iWidth = t.cam.image.size().width;

		std::cout << " od dsz   : " << t.od->dsz << std::endl;

		// check allocation , SIB
		assert(&t.od->SIBimage != NULL);

		//SIB allocation, get handle
		for (int i = 0; i < 2; i++) {
			t.od->SIBimage[i]._pMem = segment.allocate(dsz * 3);
			assert(&t.od->SIBimage[i]._pMem != NULL);
			t.od->SIBimage[i]._hMem = segment.get_handle_from_address(t.od->SIBimage[i]._pMem);
			assert(&t.od->SIBimage[i]._hMem != NULL);
		}
		//initial data copy
		memcpy(t.od->SIBimage[0]._pMem, t.cam.image.data, dsz);
		memcpy(t.od->SIBimage[1]._pMem, t.cam.image.data, dsz);

		// get handle
		managed_shared_memory::handle_t handle = segment.get_handle_from_address(t.od);

		//execute
		std::stringstream s;
		char odc_fn[] = "MFCodclient.exe";
		s << odc_fn << " " << handle << " " << t.od << std::ends;
		std::cout << "Main cmd: " << s.str() << std::endl;

		t.od->nDetections = 0;
		t.od->init_flag = false;
		t.od->index = 0;
		t.od->frame_full = false;
		t.od->ready_flag = false;
		t.od->run_flag = false;

		//RUN
		std::cout << "\t\tinitialize od\n";
		std::thread p1(program_thread, std::ref(s));
		std::thread p2(_camera, std::ref(t));

#define car_up
#define car_down

		while (1) {
			if (t.od->init_flag == false)
				continue;

			image = cv::Mat(t.cam.image);
			if (t.od->run_flag) {
				for (int i = 0; i < t.od->nDetections; i++) {
					std::cout
						<< "classes: " << t.od->classes[i] << "\t"
						<< "scores : " << t.od->scores[i] << "\t"
						<< "boxes  : " << t.od->boxes[i].x << "\t" << t.od->boxes[i].y << "\t"
						<< t.od->boxes[i].width << "\t" << t.od->boxes[i].height
						<< std::endl;
					//#ifdef person
					//            if (t.od->classes[i] == 1) {
					//               cv::rectangle(image, t.od->boxes[i], cv::Scalar(0, 0, 255), 2);
					//            }
					//#endif
#ifdef car_up 
					if ((t.od->classes[i] == 8 || t.od->classes[i] == 6 || t.od->classes[i] == 3) && (t.od->boxes[i].y <= 160) && (t.od->boxes[i].y >= 80) && (t.od->boxes[i].x <= 300)) {
						cv::rectangle(image, t.od->boxes[i], cv::Scalar(0, 255, 0), 2);
						count1++;
						std::cout
							<< "UP: " << count1 << "\t"
							<< std::endl;
					} // 8 : Ʈ��, 6 : ����, 3 : �Ϲ� �ڵ���
#endif
#ifdef car_down
					if ((t.od->classes[i] == 8 || t.od->classes[i] == 6 || t.od->classes[i] == 3) && (t.od->boxes[i].y >= 185) && ((t.od->boxes[i].x <= 470))) {
						cv::rectangle(image, t.od->boxes[i], cv::Scalar(0, 255, 255), 2);
						count2++;
						std::cout
							<< "DOWN: " << count2 << "\t"
							<< std::endl;
					} // 8 : Ʈ��, 6 : ����, 3 : �Ϲ� �ڵ���
#endif
				}

				t.od->run_flag = false;
			}
			else {
				boost::detail::Sleep(50);
			}

			result = image.clone();
			//imshow("result", result);

			//t.cam.vc >> t.cam.image;
			//frame = cv::Mat(t.cam.image);
			frame = cv::Mat(result);
			if (frame.empty())
				return;

			
			String s_up = to_string(count1);
			String s_down = to_string(count2);

			CString cstr1, cstr2;
			cstr1 = CString::CStringT(CA2CT(s_up.c_str()));
			cstr2 = CString::CStringT(CA2CT(s_down.c_str()));

			SetDlgItemText(IDC_STATIC1, str1 + cstr1);
			SetDlgItemText(IDC_STATIC2, str2 + cstr2);

			CRect Bound(0, 0, 500, 300);

			BITMAPINFOHEADER bi = { sizeof(bi) };
			bi.biWidth = frame.cols;
			bi.biHeight = -frame.rows;
			bi.biBitCount = (WORD)(frame.channels() * 8);
			bi.biPlanes = 1;

			pDC = GetDlgItem(IDC_PICC)->GetDC();
			GetDlgItem(IDC_PICC)->GetClientRect(&rect);
			pDC->SetStretchBltMode(COLORONCOLOR);

			//�Ͻ����� ���� ����
			if (IsPaused == TRUE) {
				played = false;
				while (1) {
					if (IsPlayed == TRUE)
						goto ESCAPE;
					Wait(15);
				}
			}

		ESCAPE:
			played = TRUE;
			IsPaused = FALSE;
			IsPlayed = FALSE;
			StretchDIBits(pDC->GetSafeHdc(), 0, 0, Bound.right, Bound.bottom, 0, 0, frame.cols, frame.rows, frame.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS, SRCCOPY);
		}



		segment.deallocate(t.od->SIBimage[0]._pMem);
		segment.deallocate(t.od->SIBimage[1]._pMem);
		segment.deallocate(t.od);

		p1.join();
		p2.join();
		t.cam.vc.release();

	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CMFCApplication1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMFCApplication1Dlg::Wait(DWORD dwMillisecond)
{
	MSG msg;
	DWORD dwStart;
	dwStart = GetTickCount();

	while (GetTickCount() - dwStart < dwMillisecond)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

void CMFCApplication1Dlg::OnBnClickedButton3()
{
	IsPaused = TRUE;
	
}


void CMFCApplication1Dlg::OnBnClickedButton1()
{
	IsPlayed = TRUE;
}

void CMFCApplication1Dlg::RadioCtrl(UINT ID)
{
	UpdateData(TRUE);
}