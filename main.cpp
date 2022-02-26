#include <WINSOCK2.H>  
 
#include <openssl/ssl.h>
#include <openssl/err.h>
 
#include <iostream>
#include <sstream>
 
 
using namespace std;
 
int main()
{
	//����SSL������Ϣ 
	SSL_load_error_strings();
	//���SSL�ļ���/HASH�㷨 
	SSLeay_add_ssl_algorithms();
	//�ͻ��ˣ������ѡ��SSLv23_server_method() 
	const SSL_METHOD *meth = SSLv23_client_method();
	//�����µ�SSL������ 
	SSL_CTX* ctx = SSL_CTX_new(meth);
	if(ctx == NULL)
	{
		ERR_print_errors_fp(stderr);
		cout<<"SSL_CTX_new error !";
		return -1;
	}
	
	WSADATA wsaData;
	if(WSAStartup(MAKEWORD(2,2),&wsaData)!=0)
	{
		return -1;
	}
	SOCKET client = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(client == INVALID_SOCKET)
	{
		cout<<"socket error !";
		return -1;
	}
	
	string host = "www.baidu.com";
	unsigned short port = 443;
	hostent *ip = gethostbyname(host.c_str());
	
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr = *(in_addr*)ip->h_addr_list[0];
	
	if(connect(client,(sockaddr*)&sin,sizeof(sin)) == SOCKET_ERROR)
	{
		cout<<"connect error 1";
		return -1;
	}
	
	//����SSL 
	int ret;
	SSL*ssl = SSL_new(ctx);
	if(ssl == NULL)
	{
		cout<<"SSL NEW error";
		return -1;
	}
	//��SSL��TCP SOCKET ���� 
	SSL_set_fd(ssl,client);
	//SSL���� 
	ret = SSL_connect(ssl);
	if(ret == -1)
	{
		cout<<"SSL ACCEPT error ";
		return -1;
	}
	
	stringstream stream;
	stream << "GET https://"<<host<<" HTTP/1.0\r\n";
	stream << "Accept: */*\r\n";
	//stream << "Accept-Encoding: gzip, deflate, br\r\n";//��Ҫ���룬���򻹵ö�һ������Ĳ���
	stream << "Accept-Language: zh-Hans-CN, zh-Hans; q=0.8, en-US; q=0.5, en; q=0.3\r\n";
	stream << "Connection: Keep-Alive\r\n";
	stream << "Host: " << host << "\r\n";
	stream << "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/64.0.3282.140 Safari/537.36 Edge/17.17134\r\n";
	stream << "\r\n";
  
	string s = stream.str();
	const char *sendData = s.c_str();
	ret = SSL_write(ssl,sendData,strlen(sendData));
	if(ret == -1)
	{
		cout<<"SSL write error !";
		return -1;
	}
	char *rec = new char[1024*1024];
	int start = 0;
	while((ret = SSL_read(ssl,rec+start,1024))>0)
	{
		start += ret;
	}
	rec[start] = 0;
	cout<<rec;
	
	//�ر�SSL�׽��� 
	SSL_shutdown(ssl);
	//�ͷ�SSL�׽��� 
	SSL_free(ssl);
	//�ͷ�SSL�Ự���� 
	SSL_CTX_free(ctx);
	
	closesocket(client);
	WSACleanup();
}
