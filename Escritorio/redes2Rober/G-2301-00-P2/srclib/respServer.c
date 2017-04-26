#include "UsoSockets.h"
//#include "conexion.h"
void (* pcommandRespServer[numResp])(char *)={funcDefServer,funcDefServer,funcDefServer,funcDefServer,funcDefServer,rplNowAway};


void AnalizeServer(long response, char* buf){
	IRCInterface_PlaneRegisterOutMessageThread (buf);
	//if(response >301)
		//(* pcommandRespServer [response -301])(buf);
	switch(response){
		case 2:
			ServerNick(buf);
			break;
		case 9:
			ServerJoin(buf);
			break;
		case 10:
			ServerPart(buf);
			break;
		case 16:
			ServerPrivMsg(buf);
		case 34:
			ServerPing(buf);
			break;
		case 83:
			ServerWhoIsUser(buf);
			break;
		case 86:
			ServerWhoIsServer(buf);
			break;
		case 87:
			ServerWhoIsIdle(buf);
			break;
		case 88:
			ServerWho(buf);
			break;
		case 78:
			ServerEndWho(buf);
			break;
		case 84:
			ServerWhoIsChannels(buf);
			break;
		case 113:
			ServerNames(buf);
			break;
		case 117:
			ServerNoTopic(buf);
			break;
		case 124:
			ServerReplyTopic(buf);
			break;
		case 201:
			ServerTopicSetter(buf);
			break;//rply de quien ha puesto el topic
		case 182:
			ServerWhoIs(buf);
			break;
		case 79:
			ServerEndWhoIs(buf);
			break;
		case 172:
			ServerAway(buf);
			break;
		case 183:
			rplWelcome(buf);
			break;
		case 136:
			rplLuserClient(buf);
			break;
		case 135:
			rplLuserChannels(buf);
			break;
		case 140:
			rplMotd(buf);
			break;
		case 184:
			rplCreated(buf);
			break;
		case 75:
			rplMyInfo(buf);
			break;
		case 74:
			rplYourHost(buf);
			break;
		case 17:
			respNotice(buf);
			break;
		case 141:
			rplStartMotd(buf);
			break;
		case 132:
			rplEndMotd(buf);
			break;
		default:
			funcDefServer(buf);
	}
}
void ServerNoTopic(char* str){
	char *prefix,*nick,*channel,*topic,NoTopic[500];
	if(IRCParse_RplNoTopic (str, &prefix, &nick, &channel, &topic)!= IRC_OK)
		return;
	printf("%s\n",channel);
	strcat(NoTopic, channel);
	strcat(NoTopic, "No topic is set");
	IRCInterface_WriteChannelThread (channel, "*", NoTopic);
	IRC_MFree(4,&prefix, &nick, &channel, &topic);
	return;
}
void ServerPrivMsg(char * str){
	char *prefix,*msgtarget,*msg,*token,*nick,*nuser,*real,*host,*nick2,*user2, *tokenf;
	if(IRCParse_Privmsg (str, &prefix, &msgtarget, &msg)!= IRC_OK)
		return;
	
	tokenf=strtok(msg, " ");
	if(strcmp(tokenf,"/001FSEND")==0){//caso en el que el privmsg se trata del envio de un fichero
		 //"\001FSEND NOMBRE_FICHERO HOSTNAME_EMISOR PUERTO_EMISOR LONGITUD_FICHERO_BYTES"
		 
		 char *nombre_fichero, *hostname_emisor, * puerto_emisor, *longitud_fichero_bytes;
		char buf [1000];
		int puerto,socketRcv;
		long unsigned int longitud;
		char * data;
		getDataFILE(&data);
	
		 //tokenf ++;
		 tokenf=strtok(NULL, " ");
		 //tokenf ++;
		 hostname_emisor=strtok(NULL," ");
		 //hostname_emisor++;
		 puerto_emisor=strtok(NULL," ");
		 //puerto_emisor++;
		 longitud_fichero_bytes=strtok(NULL," ");
		 //longitud_fichero_bytes++;
		 
		 puerto=atoi(puerto_emisor);
		 longitud=atol(longitud_fichero_bytes);
		 
		 printf("\n\n DATOS %s %s %d %ld\n\n", tokenf, hostname_emisor, puerto, longitud);
		 
		 hostname_emisor++;
		 printf("hostname %s", hostname_emisor);
		 bzero(buf,1000);
		 
		 CrearSocketTCP(socketRcv, hostname_emisor, puerto);
		 
		 
        if(recv(&socketRcv,buf,1000,0)<=0){
            return;
        }
        
        printf("BUF %s", buf);
        
        FILE * f;
        f = fopen(tokenf,"w");
        fwrite(data,sizeof(data[0]),longitud,f);
        
		 
	 }else{
	
	
	token = strtok(str, "!");
	token ++;
	if(msgtarget[0] != '#'){
		if(IRCInterface_QueryChannelExistThread (msgtarget) != TRUE){
			char * token2, *canal,*token3;
			getNick(&nick);
			getUser(&nuser);
			getRealName(&real);
			getServer(&host);
			nick2 = strdup(token);
			if(strcmp(msgtarget,nick) != 0){
				IRCInterface_AddNewChannelThread(msgtarget, 0);
				canal = strdup(msgtarget);
			}else{
				IRCInterface_AddNewChannelThread(nick2, 0);
				canal = strdup(nick2);
			}
			
			token = strtok(NULL,"!");
			token2 = strtok(token, "@");
			token2++;
			user2 = strdup(token2);
			token3 = strtok(token2, " ");
			IRCInterface_AddNickChannelThread (canal , nick, nuser, real, host, 2);
			IRCInterface_AddNickChannelThread (canal , nick2, user2, real, token3, 2);
			free(msgtarget);
			msgtarget=strdup(canal);
			free(canal);
			}
		}
	}
	IRCInterface_WriteChannelThread (msgtarget, token, msg);
	IRC_MFree(3, &prefix, &msgtarget, &msg);
	return;
	
	
	
}
	
void ServerEndWho(char *str){
	return;
}
void ServerReplyTopic(char *str){
	char *prefix,*nick,*channel,*topic;
	if(IRCParse_RplTopic (str, &prefix, &nick, &channel, &topic)!= IRC_OK)
		return;
	//IRCInterface_SetTopic(topic);
	IRCInterface_WriteChannelThread (channel, "*", str);
	IRC_MFree(4,&prefix, &nick, &channel, &topic);
	return;
}
void ServerTopicSetter(char *str){
	char *prefix,*nick,*channel,*topic;
	if(IRCParse_RplTopic (str, &prefix, &nick, &channel, &topic)!= IRC_OK)
		return;
	IRCInterface_WriteChannelThread (channel, "*", str);
	IRC_MFree(4,&prefix, &nick, &channel, &topic);
}
void ServerWho(char *str){
	char* prefix,*nick,*channel,*user,*host,*server,*nick2,
		*type,*msg,*realname;
	int hopcount;
	if(IRCParse_RplWhoReply (str, &prefix, &nick, &channel,
							&user, &host, &server, &nick2, 
							&type,&msg,&hopcount, &realname)!= IRC_OK)
		return;
	IRCInterface_AddNickChannelThread (channel, nick2, user, realname, host, 0);
	IRC_MFree(10,&prefix, &nick, &channel,&user, &host, &server, &nick2, 
							&type,&msg, &realname);
	return;
}
void ServerWhoIsServer(char*str){
	char * prefix,*nick,*nick2,*server,*serverinfo;
	if( IRCParse_RplWhoIsServer (str, &prefix, &nick, &nick2, &server, &serverinfo) != IRC_OK)
		return;
	IRCInterface_WriteSystemThread ("*", str);
	IRC_MFree(5, &prefix, &nick, &nick2, &server, &serverinfo);
}
void ServerWhoIsChannels(char* str){
	char *prefix,*nick,*nick2,*chanelstr;	
	if(IRCParse_RplWhoIsChannels (str,&prefix, &nick, &nick2, &chanelstr) !=IRC_OK)
		return;
	IRCInterface_WriteSystemThread ("*", str);
	IRC_MFree(4,&prefix, &nick, &nick2, &chanelstr);
}
void ServerWhoIsIdle(char *str){
	char *prefix,*nick,*nick2,*msg,*timeElapse,resp[500];
	int secs_idle,signon;
	
	if(IRCParse_RplWhoIsIdle (str, &prefix, &nick, &nick2, &secs_idle, &signon, &msg)!=IRC_OK)
		return;
	timeElapse=IRCTAD_TimestampToLocalDate (signon);
	sprintf(resp,"[%s] inactividad %d (seconds idle), entró el: %s\n",nick,secs_idle,timeElapse);
	IRCInterface_WriteSystemThread ("*", resp);
	IRC_MFree(5,&prefix, &nick, &nick2,  &msg, &timeElapse);
}
void ServerPing(char * str){
	char*prefix = NULL,*server = NULL,*server2  = NULL,*msg = NULL,*command=NULL;
	if(IRCParse_Ping (str, &prefix, &server, &server2, &msg)!=IRC_OK)
		return;
	
	getServer(&server2);
	
	
	if(IRCMsg_Pong (&command,NULL,server2,server,msg)!=IRC_OK){
		if(IRCMsg_Pong (&command,NULL,server2,server,NULL)!=IRC_OK)
			return;
	}
		
	printf("%s\n",command);fflush(stdout);
	
	send(getsocketTCP(),command,strlen(command),0);
	IRC_MFree(5,&command,&prefix, &server, &server2, &msg);
	printf("Lo hise\n");fflush(stdout);
	return;
}
void funcDefServer(char * str){
	IRCInterface_WriteSystemThread ("ERR comando no conocido", str);
	return;
}
void ServerWhoIsUser(char* str){
	char* prefix,*nick,*nick2,*name,*host,*real;
	if(IRCParse_RplWhoIsUser (str, &prefix, &nick, &nick2, &name, &host, &real) != IRC_OK)
		return;  
	IRCInterface_WriteSystemThread ("*", str);	
	IRC_MFree(6, &prefix, &nick, &nick2, &name, &host, &real);
}
void ServerWhoIs(char * str){
	char* prefix,*target,*mask;
	if(IRCParse_Whois (str, &prefix, &target, &mask)!= IRC_OK)
		return;
	
	IRCInterface_WriteSystemThread ("*", str);
	IRC_MFree(3,&prefix,&mask,&target);
	return;	
}
void ServerEndWhoIs(char * str){
	
	char* prefix,*nick,*name,*msg;
	if(IRCParse_RplEndOfWhoIs (str, &prefix, &nick, &name, &msg)!= IRC_OK)
		return;
	
	IRCInterface_WriteSystemThread ("*", msg);
	IRC_MFree(3,&prefix,&nick,&name,&msg);
	return;	
}
	
void ServerJoin(char * str){
	
	char * channel, * password, *command, *nick,*nuser, *real, *host, *prefix,*msg,*token;
	
	if(IRCParse_Join(str,&prefix,&msg, &password,&channel) != IRC_OK)
		return;
	
	token = strtok(str, "!");
	token ++;
	getNick(&nick);
	if(strcmp(nick,token) == 0){
		getUser(&nuser);
		getRealName(&real);
		getServer(&host);
		if(IRCInterface_QueryChannelExistThread (channel) == TRUE){
			IRCInterface_AddNickChannelThread (channel, nick, nuser, real, host, 0);
		}
		//no existe ningun canal con ese nombre, por lo que se crea otro nuevo
		else{	
			IRCInterface_AddNewChannelThread(channel, 0);
			IRCInterface_AddNickChannelThread (channel, nick, nuser, real, host, 2);
		}
		free(nuser);free(real);free(host);
	}else{
		IRCMsg_Who (&command, NULL, channel, NULL);
		send(getsocketTCP(), command, strlen(command),0);
		IRCInterface_PlaneRegisterInMessage(command);
		free(command);
	}
	IRC_MFree(4,&prefix,&password, &nick,&msg);
	
		
}
void ServerNames(char * str){
	char *prefix,*channel,*target;
	if(IRCParse_Names (str, &prefix, &channel, &target) != IRC_OK)
		return;
	IRCInterface_WriteSystemThread ("*", str);	
	IRC_MFree(3,&prefix,&channel,&target);
	return;


}
void ServerPart(char * str){
	char *prefix,*channel,*msg, * nick, *command, *token, *mensaje;

	if(IRCParse_Part (str, &prefix, &channel, &msg) != IRC_OK){
	
		return;
	}
	getNick(&nick);
	

	
	if(strcmp(msg,"Saliendo")==0){/*Caso en el que el part es de otro cliente*/
		
		token=strtok(str,"!");
		token++;
		printf("\n nombre %s\n", token);
		IRCInterface_DeleteNickChannelThread (channel, token);
		IRCMsg_Who (&command, NULL, channel, NULL);
		send(getsocketTCP(), command, strlen(command),0);
		IRCInterface_PlaneRegisterInMessage(command);
		free(command);
		
		mensaje =(char *)malloc(sizeof(char)*100);
		strcat(mensaje, token);
		strcat(mensaje, " ha abandonado el canal ");
		strcat(mensaje,channel );
		
		IRCInterface_WriteChannelThread (channel, "*", mensaje);
		
		free(mensaje);
		
	}else{
	
	//IRCInterface_RemoveAllNicksChannelThread (channel);Se supone que deberian cerrarse todos pero no funciona
		IRCInterface_DeleteNickChannelThread (channel, nick);
	
		IRCInterface_WriteChannelThread (channel, "*", "Has abandonado el canal. (Saliendo)");
	}
	IRC_MFree(4,&prefix,&channel,&msg, &nick);
	return;


}

void ServerAway(char * str){
	char *prefix,*channel,*msg;

	if(IRCParse_Away (str, &prefix, &msg) != IRC_OK){
	
		return;
	}
	
	
	IRCInterface_WriteSystemThread ("*", "You have been marked as being away");
	
	channel= IRCInterface_ActiveChannelName();
	//no se como poner el nick en color ausente
	
	
	IRC_MFree(2,&prefix,&msg);
	return;


}
void ServerNick(char * str){
	char *prefix,*newnick, * nick, *channel, *nickaux, *msg;
	channel =NULL;

	if(IRCParse_Nick (str, &prefix, &nickaux, &newnick) != IRC_OK){
	
		return;
	}

	channel = IRCInterface_ActiveChannelName();
	
	getNick(&nick);
	
	//hacer setnick ()?
	
	IRCInterface_ChangeNickThread (nick, newnick);
	
	msg= strdup("Usted es ahora reconocido como ");
	strcat(msg, newnick);
	
	if(channel!=NULL){
		
		
		IRCInterface_WriteChannelThread (channel, "*", msg);
	}
	
	IRCInterface_WriteSystemThread ("*", msg);
	IRC_MFree(5,&prefix,&newnick, &nick, &nickaux, &msg);
	return;


}


void respNotice(char * str){
	
	char * prefix, *msgtarget, *msg, *nick;
	
	IRCParse_Notice (str, &prefix, &msgtarget, &msg);
	getNick(&nick);
	IRCInterface_WriteSystemThread("*", msg);
	
	IRC_MFree(4, &prefix, &msgtarget, &msg, &nick);
}

void rplMyInfo(char * str){
	
	char * prefix, *nick, *servername, *version, *availableusermodes, *availablechannelmodes, *addedg, *msg;
	
	//servername = getServer();
	
	IRCParse_RplMyInfo (str, &prefix,&nick,
	 &servername, &version, &availableusermodes, 
	 &availablechannelmodes, &addedg);
	 
	 //printf("servername %s, version %s availableusermodes %s availablechannelmodes %s addedg %s ",
	 //servername, version, availableusermodes, availablechannelmodes, addedg );
	 
	 msg= (char *)malloc(sizeof(char)*100);
	 
	 strcat(msg, servername);
	 strcat(msg, " ");
	 strcat(msg, version);
	 strcat(msg, " ");
	 strcat(msg, availableusermodes);
	 strcat(msg, " ");
	 strcat(msg, availablechannelmodes);
	 
	 //printf("MENSAJE %s", msg);
	 	
	IRCInterface_WriteSystemThread("*", msg);
	IRC_MFree(8,&msg, &prefix, &nick, &servername, &version, &availableusermodes, &availablechannelmodes, &addedg);
	
}
void rplLuserClient(char * str){
	
	char * prefix, *nick, *msg;
	int  nusers, ninvisibles, nservers;
	//servername = getServer();
	
	
	IRCParse_RplLuserClient (str, &prefix, &nick, &msg,
	 &nusers, &ninvisibles, &nservers);
	
		printf("MENSAJE %s, ", msg);
	
	IRCInterface_WriteSystemThread("*", msg);
	IRC_MFree(3, &prefix, &nick, &msg);
	
	
}
void rplLuserChannels(char * str){
	
	char * prefix, *nick, *msg;
	int  nchannels;
	//servername = getServer();
	
	
	IRCParse_RplLuserChannels (str, &prefix, &nick, &nchannels, &msg);
	
	
	
	IRCInterface_WriteSystemThread("*", msg);
	IRC_MFree(3, &prefix, &nick, &msg);
	
	
}

void rplMotd(char * str){
	
	char * prefix, *nick, *msg;
	 IRCParse_RplMotd (str, &prefix, &nick, &msg);
	 
	IRCInterface_WriteSystemThread("*", msg);
	IRC_MFree(3, &prefix, &nick, &msg);
	
}
void rplEndMotd(char * str){
	
	char * prefix, *nick, *msg;
	 IRCParse_RplEndOfMotd(str, &prefix, &nick, &msg);
	 printf("MENSAJE %s", msg);
	IRCInterface_WriteSystemThread("*", msg);
	IRC_MFree(3, &prefix, &nick, &msg);
	
	
}

void rplStartMotd(char * str){
	
	char * prefix, *nick, *msg, * server;
	 IRCParse_RplMotdStart (str, &prefix, &nick, &msg, &server);
	 
	IRCInterface_WriteSystemThread("*", msg);
	IRC_MFree(4, &prefix, &nick, &msg, &server);
	
	
}
void rplWelcome(char * str){
	
	char * prefix, *nick, * msg, *token;
	
	IRCParse_RplWelcome (str, &prefix, &nick, &msg);
	
	/*es posible que no sea necesario ni parsear el welcome puesto que si se recibe esto indica que el usuario ya se ha registrado correctamente*/
	/*tal vez habria que crear el usuario aqui */
	IRCInterface_WriteSystemThread ("*", msg);
	
	token = strtok(msg, "!");
	token = strtok(NULL, "!");
	
	
	setPrefix(token);
	
	free(prefix);
	free(nick);
	free(msg);
}

void rplYourHost(char * str){
	
	char * prefix, *nick, * msg, *servername, * versionname;
	
	IRCParse_RplYourHost (str, &prefix, &nick, &msg, &servername, &versionname);
	
	/*es posible que no sea necesario ni parsear el welcome puesto que si se recibe esto indica que el usuario ya se ha registrado correctamente*/
	/*tal vez habria que crear el usuario aqui */
	IRCInterface_WriteSystemThread ("*", msg);
	
	IRC_MFree(2, &servername, &versionname);
	free(prefix);
	free(nick);
	free(msg);
	
	
}
void rplCreated(char * str){
	
	char * prefix, *nick, * msg, *timedate;
	
	IRCParse_RplCreated(str, &prefix, &nick, &timedate, &msg);
		
	
	printf("Mensaje en created %s ", msg);
	IRCInterface_WriteSystemThread ("*", msg);
	
	free(timedate);
	free(prefix);
	free(nick);
	free(msg);
	
	
}

void rplNowAway(char * str){
	
	//llamar a la funcion que ponga el nombre del cliente en un tono mas claro
	
}

void* mandarFichero(void *arg){
	
	struct sockaddr_in * clienaddr = (struct sockaddr_in *)arg;
	
    socklen_t clilen;
	
	int socketTCPFILE;
	char * dataFile;
	long unsigned int length;
	
	length = getLenght();
	socketTCPFILE=getsocketTCPFILE();
	
	
	int connf = accept(socketTCPFILE,(struct sockaddr *) clienaddr, &clilen);
	
	getDataFILE(&dataFile);
	
	send(connf, dataFile, length, 0);
	
	
}










