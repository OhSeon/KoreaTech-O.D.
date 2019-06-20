
// MFCApplication1Dlg.h : ��� ����
//

#pragma once
#include "afxwin.h"
#include <sstream>
#include <cstdlib> //std::system
#include <thread>
#include "SharedImageBuffer.h"
#include <boost/interprocess/sync/scoped_lock.hpp>
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;



// CMFCApplication1Dlg ��ȭ ����
class CMFCApplication1Dlg : public CDialogEx
{
// �����Դϴ�.
public:
	CMFCApplication1Dlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.
	void Wait(DWORD dwMillisecond);
	bool IsPaused = FALSE;
	bool IsPlayed = FALSE;
	UINT m_radio;

	cv::Mat image, result;
	Mat frame;
	ti t;
	bool m_bView;

	int nDetections;
	std::vector<cv::Rect> boxes;
	std::vector<float> scores;
	std::vector<int> classes;
	std::stringstream s;

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCAPPLICATION1_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CStatic m_picc;
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton1();
	afx_msg void RadioCtrl(UINT ID);
};
