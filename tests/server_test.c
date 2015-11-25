/******************************************************************************
 *                AtomWeb: An embedded web server                             *
 *                                                                            *
 * Copyright (c) 2015 Gong Ke                                                 *
 * All rights reserved.                                                       *
 *                                                                            *
 * Redistribution and use in source and binary forms, with or without         *
 * modification, are permitted provided that the following conditions are     *
 * met:                                                                       *
 * 1.Redistributions of source code must retain the above copyright notice,   *
 * this list of conditions and the following disclaimer.                      *
 * 2.Redistributions in binary form must reproduce the above copyright        *
 * notice, this list of conditions and the following disclaimer in the        *
 * documentation and/or other materials provided with the distribution.       *
 * 3.Neither the name of the Gong Ke; nor the names of its contributors may   *
 * be used to endorse or promote products derived from this software without  *
 * specific prior written permission.                                         *
 *                                                                            *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS    *
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,  *
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR     *
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR           *
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,      *
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,        *
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR         *
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF     *
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING       *
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS         *
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.               *
 *****************************************************************************/

#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <atomweb.h>

#ifdef AW_MINGW
#include <winsock2.h>
#include <ws2tcpip.h>

typedef int socklen_t;
#endif

#ifdef AW_LINUX
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#ifdef AW_LINUX
static void
handler(int sig)
{
}
#endif

int
main (int argc, char **argv)
{
#ifdef AW_LINUX
	struct sigaction sa;
#endif
#ifdef AW_MINGW
	WSADATA wsa_data;
#endif
	struct sockaddr_in addr;
	AW_Server *serv;
	int serv_sock = -1;
	int port = 80;
	int r;
	extern const AW_Map *aw_map;
	
#ifdef AW_LINUX
	sa.sa_handler = handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	sigaction(SIGINT, &sa, NULL);
#endif

#ifdef AW_MINGW
	if(WSAStartup(MAKEWORD(2, 0), &wsa_data) != 0){
		fprintf(stderr, "winsock2 startup failed\n");
		return 1;
	}
#endif

	/*Get port number*/
	if (argc > 1) {
		long int pn;

		pn = strtol(argv[1], NULL, 0);
		if (pn < 65536)
			port = pn;
	}

	/*Create server*/
	serv = aw_server_create();
	aw_server_set_map(serv, aw_map);

	serv_sock = socket(AF_INET, SOCK_STREAM, 0);

	addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htons(INADDR_ANY);
    addr.sin_port = htons(port);

	r = bind(serv_sock, (struct sockaddr*)&addr, sizeof(addr));
	if (r == -1) {
		fprintf(stderr, "bind to port %d failed\n", port);
		goto end;
	}

	fprintf(stderr, "bind to port %d\n", port);

	r = listen(serv_sock, 5);
	if (r == -1) {
		fprintf(stderr, "listen failed\n");
		goto end;
	}

	fprintf(stderr, "listen\n");

	while (1) {
		struct sockaddr_in client_addr;
		socklen_t len = sizeof(client_addr);
		int sess_sock;
		AW_Session *sess;

		sess_sock = accept(serv_sock, (struct sockaddr*)&client_addr,
					&len);
		if (sess_sock < 0) {
			if (errno == EINTR)
				break;

			fprintf(stderr, "accept failed\n");
			continue;
		}

		fprintf(stderr, "accept a client\n");

		sess = aw_session_create(serv, (AW_Ptr)(AW_IntPtr)sess_sock);
		aw_session_run(sess);
		aw_session_destroy(sess);

		fprintf(stderr, "disconnect\n");
		close(sess_sock);
	}

end:
	/*Release*/
	fprintf(stderr, "close the server\n");
	close(serv_sock);
	aw_server_destroy(serv);

#ifdef AW_MINGW
	WSACleanup();
#endif
	return 0;
}
