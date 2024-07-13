// main
package com.cyocum;

import fi.iki.elonen.NanoHTTPD;
import fi.iki.elonen.NanoHTTPD.IHTTPSession;
import fi.iki.elonen.NanoHTTPD.Response;
import java.io.IOException;


public class CYocumServer extends NanoHTTPD {

    private JDBCConnection connection;

    public CYocumServer() throws IOException {
        super(8080);
        connection = new JDBCConnection();
        start(NanoHTTPD.SOCKET_READ_TIMEOUT, false);
        System.out.println("\nRunning! Point your browsers to http://3.139.156.162:8080/ \n");
    }

    public static void main(String[] args) {
        try {
            // create new instance of server
            new CYocumServer();
        } catch (IOException ioe) {
            System.err.println("Server failed to start:" + ioe + "\n");
        }
    }

    // calls Util.java functions
    @Override
    public Response serve(IHTTPSession session) {
        if (session.getMethod() == Method.GET) {
            return Util.performGet(connection, session);
        } else if (session.getMethod() == Method.POST) {
            return Util.performPost(connection, session);
        } else if (session.getMethod() == Method.PUT) {
            return Util.performPost(connection, session);
        } else if (session.getMethod() == Method.DELETE) {
            return Util.performDelete(connection, session);
        }

        return Util.failedAttempt();
    }
}
