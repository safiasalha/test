import java.net.*;

     import java.io.*;

  

     public class smtptest {

  

       //

       // Java main application for the SMTP test.

       //

       public static void main( String[] args ) {

 

        // Create a new smtpc object

        smtpc mail = new smtpc( 

            "The Subject",

            "tim@mtjones.com", "mtj@mtjones.com", "text/html","<HTML><BODY><H1>This is the mail"+"</H1></BODY></HTML>" );


        // Send the previously created e-mail

        mail.send();

 

      }

 

    }

 

    class smtpc {
private String s_subject;

      private String s_sender;

      private String s_recipient;

      private String s_content_type;

      private String s_contents;

      private PrintStream out;

      private BufferedReader inp;

 

      // The outgoing SMTP server

      private String mailserver = "192.168.1.1";
public smtpc( String subject, String sender, 

                    String recipient, String content_type,

                    String contents ) {

 

        s_subject = subject;

        s_sender = sender;

        s_recipient = recipient;

        s_content_type = content_type;

        s_contents = contents;

 

      }
class MyException extends Exception {

        public MyException() { super(); }

      }

 private void dialog( String command, String expected_response ) throws MyException {

 

        // If a command is available, send it to the server

        if ( command != null ) {

 out.print( command );
}

if ( expected_response != null ) {

 

          try {

 

            // Get a line from the SMTP server through 

            // the socket

            String line = inp.readLine();

 

           // Check the status code

            if (expected_response.equalsIgnoreCase(line.substring( 0, expected_response.length() ) ) ) {

 

              return;

 

            } else {

  throw new MyException();



           }



         } catch( IOException e ) {



             // Another error occurred, throw an exception

             throw new MyException();


         }


       }



     }





     public void send () {



       Socket sock;



       try {
 sock = new Socket( mailserver, 25 );



         // Create the input stream

         inp = new BufferedReader(new InputStreamReader(sock.getInputStream() ) );



         // Create the output stream

         out = new PrintStream( sock.getOutputStream() );



         try {

 this.dialog( null, "220" );

 this.dialog( "HELO thisdomain.com\n", "250" );

String mailFrom = "MAIL FROM:<" + s_sender + ">\n";

       this.dialog( mailFrom, "250" );

String rcptTo = "RCPT TO:<" + s_recipient + ">\n";

           this.dialog( rcptTo, "250" );

 this.dialog( "DATA\n", "354" );

String from = "From: " + s_sender + "\n";

           this.dialog( from, null );

  String recip = "To: " + s_recipient + "\n";

           this.dialog( recip, null );


String subject = "Subject: " + s_subject + "\n";

           this.dialog( subject, null );


String ct = "Content-Type: " + s_content_type + "\n";

           this.dialog( ct, null );

this.dialog( s_contents, null );


 this.dialog( "\n.\n", "250" );

 this.dialog( "QUIT\n", "221" );

 sock.close();


         } catch( MyException e ) {



           System.out.println( e );



         }

       } 

       catch( UnknownHostException e ) {}

       catch( IOException e ) {}



     }



   }
