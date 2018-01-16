# C-Socket-Practice

윤성우 열혈 TCP/IP 스터디하는 저장소입니다.  

아래는 관련 레퍼런스를 링크합니다.
### Linux
[socket](http://man7.org/linux/man-pages/man2/socket.2.html)  
- int socket(int domain, int type, int protocol);  
   성공 시 파일 디스크립터, 실패 시 -1 반환  

[bind](http://man7.org/linux/man-pages/man2/bind.2.html)  
[listen](http://man7.org/linux/man-pages/man2/listen.2.html)  
[accept](http://man7.org/linux/man-pages/man2/accept.2.html)  
[connect](http://man7.org/linux/man-pages/man2/connect.2.html)  

리눅스에서의 소켓조작은 파일조작과 동일하게 간주된다. 리눅스는 소켓을 파일의 일종으로 구분한다.  
[open](http://man7.org/linux/man-pages/man2/open.2.html)  
[write](http://man7.org/linux/man-pages/man2/write.2.html)  
[read](http://man7.org/linux/man-pages/man2/read.2.html)  

### Windows  
윈속(윈도우 소켓)을 기반으로 프로그램을 개발하기 위해서는 기본적으로 다음 두 가지를 진행해야 한다.
- 헤더파일 winsock2.h를 포함한다.  
- ws2_32.lib 라이브러리를 링크시켜야 한다.  
  - 프로젝트 단위 링크 방법: 프로젝트 '속성' - '구성 속성' - '링커' - '입력' - '추가 종속성' - 'ws2_32.lib' 추가.  

[socket](https://msdn.microsoft.com/en-us/library/windows/desktop/ms740506(v=vs.85).aspx)  
