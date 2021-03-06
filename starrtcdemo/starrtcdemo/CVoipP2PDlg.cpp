// CVoipP2PDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "starrtcdemo.h"
#include "CVoipP2PDlg.h"
#include "afxdialogex.h"
#include "CUtil.h"
#include <Winsock2.h>
#include<ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib ")  //linking to the library
// CVoipP2PDlg 对话框

IMPLEMENT_DYNAMIC(CVoipP2PDlg, CDialogEx)

CVoipP2PDlg::CVoipP2PDlg(CUserManager* pUserManager, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_VOIP_P2P, pParent)
{
	m_strLocalIP = "";
	m_strAimIP = "";
	m_pUserManager = pUserManager;
	m_pVoipManager = new XHVoipP2PManager(this);
	m_ShowLiveDlg.addPictureControlListener(this);
	m_ShowLiveDlg.addShowLiveListener(this);
	m_bConnect = false;
	m_pSoundManager = new CSoundManager(this);
}

CVoipP2PDlg::~CVoipP2PDlg()
{
}

void CVoipP2PDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS_LOCAL_IP, m_LocalIP);
	DDX_Control(pDX, IDC_IPADDRESS_AIM_IP, m_AimIP);
}


BEGIN_MESSAGE_MAP(CVoipP2PDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_CALLING, &CVoipP2PDlg::OnBnClickedButtonCalling)
END_MESSAGE_MAP()


// CVoipP2PDlg 消息处理程序


void CVoipP2PDlg::OnBnClickedButtonCalling()
{
	BYTE nf1, nf2, nf3, nf4;
	m_AimIP.GetAddress(nf1, nf2, nf3, nf4);
	if (nf4 == 0)
	{
		AfxMessageBox("请确认对方IP");
		return;
	}
	CString str;
	str.Format("%d.%d.%d.%d", nf1, nf2, nf3, nf4);//这里的nf得到的值是IP值了. 
	m_strAimIP = str.GetBuffer(0);

	//呼叫对方
	if (m_pVoipManager != NULL)
	{
		m_pVoipManager->call(m_strAimIP);
	}

	CRect rect;
	::GetWindowRect(this->m_hWnd, rect);
	m_ShowLiveDlg.MoveWindow(CRect(rect.left-100, rect.top-200, rect.right+100, rect.bottom+ 200), true);
	m_ShowLiveDlg.ShowWindow(SW_SHOW);

	if (m_ShowLiveDlg.m_pDataShowView != NULL)
	{
		m_ShowLiveDlg.m_pDataShowView->addUser(m_pUserManager->m_ServiceParam.m_strUserId, true);
		m_ShowLiveDlg.m_pDataShowView->setShowPictures();
	}

	startGetData((CROP_TYPE)m_pUserManager->m_ServiceParam.m_CropType, false);
}


BOOL CVoipP2PDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_ShowLiveDlg.Create(IDD_DIALOG_SHOW_LIVE, this);
	WSADATA wsaData = { 0 };
	if (WSAStartup(MAKEWORD(2, 1), &wsaData) == 0)
	{
		char szHostName[MAX_PATH] = { 0 };
		int nRetCode;
		nRetCode = gethostname(szHostName, sizeof(szHostName));
		PHOSTENT hostinfo;
		if (nRetCode == 0)
		{
			hostinfo = gethostbyname(szHostName);
			m_strLocalIP = inet_ntoa(*(struct in_addr*)*hostinfo->h_addr_list);		
		}
		WSACleanup();
	}
		
	
	m_pVoipManager->setUserId(m_strLocalIP);

	//m_strLocalIP = CUtil::GetLocalIpAddress();
	int pos = m_strLocalIP.find(".");
	string strVal1 = m_strLocalIP.substr(0, pos);
	m_strLocalIP = m_strLocalIP.substr(pos + 1, m_strLocalIP.length() - pos - 1);
	pos = m_strLocalIP.find(".");
	string strVal2 = m_strLocalIP.substr(0, pos);
	m_strLocalIP = m_strLocalIP.substr(pos + 1, m_strLocalIP.length() - pos - 1);
	pos = m_strLocalIP.find(".");
	string strVal3 = m_strLocalIP.substr(0, pos);
	m_strLocalIP = m_strLocalIP.substr(pos + 1, m_strLocalIP.length() - pos - 1);
	string strVal4 = m_strLocalIP;

	int val1 = atoi(strVal1.c_str());
	int val2 = atoi(strVal2.c_str());
	int val3 = atoi(strVal3.c_str());
	int val4 = atoi(strVal4.c_str());
	m_LocalIP.SetAddress(val1, val2, val3, val4);
	m_AimIP.SetAddress(val1, val2, val3, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

// voip callback
/**
 * 被叫方响应
 * 被叫方收到主叫方的呼叫
 * @param fromID
 */
void CVoipP2PDlg::onCalling(string fromID)
{
	CString str;
	str.Format("是否同意用户:%s请求视频通话", fromID.c_str());
	if (IDYES == AfxMessageBox(str, MB_YESNO))
	{
		m_strAimIP = fromID;
		CRect rect;
		::GetWindowRect(this->m_hWnd, rect);
		m_ShowLiveDlg.MoveWindow(CRect(rect.right, rect.top, rect.right + rect.Width() / 2, rect.bottom), true);
		m_ShowLiveDlg.ShowWindow(SW_SHOW);
		m_pVoipManager->accept(fromID);
		m_bConnect = true;

		if (m_ShowLiveDlg.m_pDataShowView != NULL)
		{
			m_ShowLiveDlg.m_pDataShowView->addUser(m_pUserManager->m_ServiceParam.m_strUserId, false);
			m_ShowLiveDlg.m_pDataShowView->addUser(fromID, true);
			m_ShowLiveDlg.m_pDataShowView->setShowPictures();
		}

		startGetData((CROP_TYPE)m_pUserManager->m_ServiceParam.m_CropType, true);
		if (m_pSoundManager != NULL)
		{
			m_pSoundManager->startSoundData(true);
		}
	}
	else
	{
		m_pVoipManager->refuse();
	}
}

/**
 * 被叫方响应
 * 主叫方在被叫方接听之前挂断（通话被取消）
 * @param fromID
 */
void CVoipP2PDlg::onCancled(string fromID)
{
	stopGetData();
	if (m_ShowLiveDlg.m_pDataShowView != NULL)
	{
		m_ShowLiveDlg.m_pDataShowView->removeAllUser();
		m_ShowLiveDlg.m_pDataShowView->setShowPictures();
	}
	m_ShowLiveDlg.ShowWindow(SW_HIDE);
	m_strAimIP = "";
}

/**
 * 主叫方响应
 * 被叫方拒绝接通（通话被拒绝）
 * @param fromID
 */
void CVoipP2PDlg::onRefused(string fromID)
{
	stopGetData();
	if (m_ShowLiveDlg.m_pDataShowView != NULL)
	{
		m_ShowLiveDlg.m_pDataShowView->removeAllUser();
		m_ShowLiveDlg.m_pDataShowView->setShowPictures();
	}
	m_ShowLiveDlg.ShowWindow(SW_HIDE);
	m_strAimIP = "";
	AfxMessageBox("对方拒绝接通");
}

/**
 * 主叫方响应
 * 被叫方线路忙（对方通话中）
 * @param fromID
 */
void CVoipP2PDlg::onBusy(string fromID)
{
	//关闭
	stopGetData();
	if (m_ShowLiveDlg.m_pDataShowView != NULL)
	{
		m_ShowLiveDlg.m_pDataShowView->removeAllUser();
		m_ShowLiveDlg.m_pDataShowView->setShowPictures();
	}
	m_ShowLiveDlg.ShowWindow(SW_HIDE);
	m_strAimIP = "";
	AfxMessageBox("对方线路忙");
}

/**
 * 主叫方响应
 * 被叫方接通（通话开始）
 * @param fromID
 */
void CVoipP2PDlg::onConnected(string fromID)
{
	//添加显示对方界面
	if (m_ShowLiveDlg.m_pDataShowView != NULL)
	{
		m_strAimIP = fromID;
		m_ShowLiveDlg.m_pDataShowView->addUser(fromID, true);
		m_ShowLiveDlg.m_pDataShowView->changeShowStyle(m_pUserManager->m_ServiceParam.m_strUserId, false);
		m_ShowLiveDlg.m_pDataShowView->setShowPictures();
	}
	//设置插入数据
	m_bInsertData = true;
	if (m_pSoundManager != NULL)
	{
		m_pSoundManager->startSoundData(true);
	}
	m_bConnect = true;
}

/**
 * 对方已经挂断
 * @param fromID
 */
void CVoipP2PDlg::onHangup(string fromID)
{
	stopGetData();
	if (m_pSoundManager != NULL)
	{
		m_pSoundManager->stopSoundData();
	}

	if (m_pVoipManager != NULL)
	{
		if (m_bConnect)
		{
			m_pVoipManager->hangup();
		}
		else
		{
			m_pVoipManager->cancel();
		}
		m_bConnect = false;
	}
	if (m_ShowLiveDlg.m_pDataShowView != NULL)
	{
		m_ShowLiveDlg.m_pDataShowView->removeAllUser();
		m_ShowLiveDlg.m_pDataShowView->setShowPictures();
	}
	m_ShowLiveDlg.ShowWindow(SW_HIDE);
	m_strAimIP = "";
}

/**
 * voip报错
 * @param errorCode
 */
void CVoipP2PDlg::onError(string errorCode)
{
	//开启自己
	stopGetData();
	if (m_pSoundManager != NULL)
	{
		m_pSoundManager->stopSoundData();
	}
	if (m_pVoipManager != NULL)
	{
		m_pVoipManager->hangup();
	}
	if (m_ShowLiveDlg.m_pDataShowView != NULL)
	{
		m_ShowLiveDlg.m_pDataShowView->removeAllUser();
	}
	m_ShowLiveDlg.ShowWindow(SW_HIDE);
	//断开连接
	CString strErr;
	strErr.Format("err:%s", errorCode.c_str());
	AfxMessageBox(strErr);
	m_strAimIP = "";
}

/**
 * 收到实时数据
 * @param data
 */
void CVoipP2PDlg::onReceiveRealtimeData(uint8_t* data, int length)
{
}

int CVoipP2PDlg::getVideoRaw(string strUserId, int w, int h, uint8_t* videoData, int videoDataLen)
{
	if (m_ShowLiveDlg.m_pDataShowView != NULL)
	{
		m_ShowLiveDlg.m_pDataShowView->drawPic(FMT_YUV420P, m_strAimIP, w, h, videoData, videoDataLen);
	}
	return 0;
}

void CVoipP2PDlg::changeShowStyle(string strUserId)
{
	if (m_ShowLiveDlg.m_pDataShowView != NULL)
	{
		string strId = m_ShowLiveDlg.m_pDataShowView->changeShowStyle(strUserId, true);
		m_ShowLiveDlg.m_pDataShowView->changeShowStyle(strId, false);
		m_ShowLiveDlg.m_pDataShowView->setShowPictures();
	}
}

void CVoipP2PDlg::closeCurrentLive()
{
}

void CVoipP2PDlg::stopLive()
{
	stopGetData();
	if (m_pSoundManager != NULL)
	{
		m_pSoundManager->stopSoundData();
	}

	if (m_pVoipManager != NULL)
	{
		if (m_bConnect)
		{
			m_pVoipManager->hangup();
		}
		else
		{
			m_pVoipManager->cancel();
		}
		m_bConnect = false;
	}
	if (m_ShowLiveDlg.m_pDataShowView != NULL)
	{
		m_ShowLiveDlg.m_pDataShowView->removeAllUser();
		m_ShowLiveDlg.m_pDataShowView->setShowPictures();
	}
	m_ShowLiveDlg.ShowWindow(SW_HIDE);
	m_strAimIP = "";
}

void CVoipP2PDlg::getLocalSoundData(char* pData, int nLength)
{
	if (m_pVoipManager != NULL)
	{
		m_pVoipManager->insertAudioRaw((uint8_t*)pData, nLength);
	}
}

void CVoipP2PDlg::querySoundData(char** pData, int* nLength)
{
	if (m_pVoipManager != NULL)
	{
		m_pVoipManager->querySoundData((uint8_t**)pData, nLength);
	}
}

void CVoipP2PDlg::addUpId()
{
	if (m_ShowLiveDlg.m_pDataShowView != NULL)
	{
		m_ShowLiveDlg.m_pDataShowView->addUser(m_pUserManager->m_ServiceParam.m_strUserId, false);
		m_ShowLiveDlg.m_pDataShowView->setShowPictures();
	}
}

void CVoipP2PDlg::insertVideoRaw(uint8_t* videoData, int dataLen, int isBig)
{
	if (m_pVoipManager != NULL)
	{
		m_pVoipManager->insertVideoRaw(videoData, dataLen, isBig);
	}
}

int CVoipP2PDlg::cropVideoRawNV12(int w, int h, uint8_t* videoData, int dataLen, int yuvProcessPlan, int rotation, int needMirror, uint8_t* outVideoDataBig, uint8_t* outVideoDataSmall)
{
	int ret = 0;
	if (m_pVoipManager != NULL)
	{
		ret = m_pVoipManager->cropVideoRawNV12(w, h, videoData, dataLen, (int)CROP_TYPE::VIDEO_CROP_CONFIG_480BW_480BH_SMALL_NONE, 0, 0, outVideoDataBig, outVideoDataSmall);
	}
	return ret;
}

void CVoipP2PDlg::drawPic(YUV_TYPE type, int w, int h, uint8_t* videoData, int videoDataLen)
{
	if (m_ShowLiveDlg.m_pDataShowView != NULL)
	{
		m_ShowLiveDlg.m_pDataShowView->drawPic(type, m_pUserManager->m_ServiceParam.m_strUserId, w, h, videoData, videoDataLen);
	}
}
