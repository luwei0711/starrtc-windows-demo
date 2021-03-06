#pragma once
#include "XHSuperRoomManager.h"
#include "CConfigManager.h"
#include "CSoundManager.h"
#include "ISuperRoomManagerListener.h"
#include "CLiveProgram.h"
#include "IButtonCallback.h"
#include "CCustomButton.h"
#include <vector>
// CSuperAudioChatroomDlg 对话框

class CSuperAudioChatroomDlg : public CDialogEx, public ISuperRoomManagerListener, public IChatroomGetListListener, public ISoundCallback, public IButtonCallback
{
	DECLARE_DYNAMIC(CSuperAudioChatroomDlg)

public:
	CSuperAudioChatroomDlg(CUserManager* pUserManager, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSuperAudioChatroomDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SUPER_AUDIO_CHATROOM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	void setConfig(CConfigManager* pConfig);
	/*
	 * 获取音频聊天室列表
	 */
	void getSuperRoomList();

	void updateUpUser();
	virtual BOOL OnInitDialog();
	CLiveProgram* getLiveProgram(CString strId, CString strName, CString strCreator);
	afx_msg void OnBnClickedButtonSuperAudioChatroomListbrush();
	afx_msg void OnBnClickedButtonCreateSuperAudioChatroom();
	afx_msg void OnNMClickListcontrolSuperAudioChatroomList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonSuperAudioChatroomSendMsg();
	afx_msg void OnBnClickedButtonSendSuperAudio();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	/**
	 * 查询聊天室列表回调
	 */
	virtual int chatroomQueryAllListOK(list<ChatroomInfo>& listData);

public:
	/**
	 * 有新用户加入会议
	 * @param meetingID 会议ID
	 * @param userID 新加入者ID
	 */
	virtual void onJoined(string meetID, string userID);

	/**
	 * 有人离开会议
	 * @param meetingID 会议ID
	 * @param userID 离开者ID
	 */
	virtual void onLeft(string meetingID, string userID);

	/**
	 * 一些异常情况引起的出错，请在收到该回调后主动断开会议
	 * @param id 
	 * @param error 错误信息
	 */
	virtual void onError(string id, string error);


	/**
	 * 聊天室成员数变化
	 * @param number 变化后的会议人数
	 */
	virtual void onMembersUpdated(int number);

	/**
	 * 自己被踢出聊天室
	 */
	virtual void onSelfKicked();

	/**
	 * 自己被禁言
	 */
	virtual void onSelfMuted(int seconds);

	/**
	 * 连麦者的连麦被强制停止
	 * @param liveID 直播ID
	 */
	virtual void onCommandToStopPlay(string liveID);

	/**
	 * 收到消息
	 * @param message
	 */
	virtual void onReceivedMessage(CIMMessage* pMessage);

	/**
	 * 收到私信消息
	 * @param message
	 */
	virtual void onReceivePrivateMessage(CIMMessage* pMessage);
	virtual int getRealtimeData(string strUserId, uint8_t* data, int len);
	/**
	 * 收到视频数据
	 * @param upId
	 * @param w 宽度
	 * @param h 高度
	 * @param videoData 数据
	 * @param videoDataLen 数据长度
	 */
	virtual int getVideoRaw(string strUserId, int w, int h, uint8_t* videoData, int videoDataLen);
public:
	virtual void getLocalSoundData(char* pData, int nLength);
	virtual void querySoundData(char** pData, int* nLength);
public:
	virtual void OnLButtonDownCallback();
	virtual void OnLButtonUpCallback();
public:
	CListCtrl m_List;
	CStatic m_SuperAudioCharroomName;
	CListBox m_MsgList;
	CEdit m_SendMsg;
	XHSuperRoomManager* m_pXHSuperRoomManager;
	CUserManager* m_pUserManager;
	CConfigManager* m_pConfig;
	CSoundManager* m_pSoundManager;
	vector<CLiveProgram> mVLivePrograms;
	CLiveProgram* m_pCurrentLive;
	CCustomButton m_AudioButton;
	CStatic m_User1;
	CStatic m_User2;
	CStatic m_User3;
	CStatic m_User4;
	CStatic m_User5;
	CStatic m_User6;
	CStatic m_User7;

	CStatic* m_UserIdArr[7];

	vector<string> m_UpUsers;
};
