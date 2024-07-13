package com.cyocum;

import com.google.gson.Gson;
import com.cyocum.classes.Window;
import fi.iki.elonen.NanoHTTPD;
import java.io.IOException;
import java.util.HashMap;
import java.util.List;

import static fi.iki.elonen.NanoHTTPD.MIME_PLAINTEXT;
import static fi.iki.elonen.NanoHTTPD.newFixedLengthResponse;


public final class Util {
    // final -> makes non-access modifier for classes

    private static final String NO_RESOURCE = "The table is empty!\n";
        // static if in separate file, else private -> same file

    private Util() {
    }

    public static NanoHTTPD.Response performGet(JDBCConnection connection, NanoHTTPD.IHTTPSession session) {
        String jsonResp = null;
        String param = getIndex(session.getUri());
        Gson gson = new Gson();

        if (param != null && !param.equals("")) {
            Window window = connection.getConsole(param);
            if (window == null) {
                return failedAttempt();
            }
            jsonResp = gson.toJson(window);
        } else {
            List<Window> consoles = connection.getConsoles();           // to JDCConnection 
            if (consoles.isEmpty()) {
                return failedAttempt();
            }
            jsonResp = gson.toJson(consoles);
        }

        return newFixedLengthResponse(jsonResp + "\n");         // \n for adjusting output on Terminal
    }

    public static NanoHTTPD.Response performPost(JDBCConnection connection, NanoHTTPD.IHTTPSession session) {
        try {
            session.parseBody(new HashMap<>());
            String result = connection.addConsole(session.getQueryParameterString());
            return newFixedLengthResponse(result + "\n");
        } catch (IOException | NanoHTTPD.ResponseException e) {
            return failedAttempt();
        }
    }

    public static NanoHTTPD.Response performDelete(JDBCConnection connection, NanoHTTPD.IHTTPSession session) {
        String result = connection.deleteConsole(getIndex(session.getUri()));
        return newFixedLengthResponse(result + "\n");
    }

    public static NanoHTTPD.Response failedAttempt() {
        return newFixedLengthResponse(NanoHTTPD.Response.Status.NOT_FOUND, MIME_PLAINTEXT,
                NO_RESOURCE);
    }

    
    // VS Code lies, it's needed
    private static String getIndex(String param) {
        return param.replaceAll("[^0-9]", "");
    }
        
}
