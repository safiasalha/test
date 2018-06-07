import java.net.*;

import javav.io.*;

public class client{

public static void main (String [] args) throws Exception{

ServerSocket s = new ServerSocket(3333);

while(true) {

Socket connectionsocket=s.accept();

BufferedReader messagefromloadbalancer=new BufferedReader(new 

InputStreamReader(connectionsocket.getInputStream());

String sentence = messagefromloadbalancer.readLine();

System.out.println("Receive Connection from the Client:" + sentence);

}
