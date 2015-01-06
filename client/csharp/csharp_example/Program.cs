using System;
using System.Net;
using System.Net.Sockets;
using System.Text;

public class SynchronousSocketClient
{

    public static void StartClient() {
        byte[] bytes = new byte[1024];

        try {
            IPHostEntry ipHostInfo = Dns.GetHostEntry(Dns.GetHostName());
            IPAddress ipAddress = null;
            foreach (IPAddress ip in ipHostInfo.AddressList)
            {
                if (ip.AddressFamily == AddressFamily.InterNetwork)
                {
                    ipAddress = ip;
                }
            }
            IPEndPoint remoteEP = new IPEndPoint(ipAddress,1986);

            Socket sender = new Socket(AddressFamily.InterNetwork, 
                SocketType.Stream, ProtocolType.Tcp );

            try {
                sender.Connect(remoteEP);
                Console.WriteLine("Socket connected to {0}",
                    sender.RemoteEndPoint.ToString());

                // create p&o request for just sensor 1
                byte[] po_request = new byte[3] { Convert.ToByte(true), Convert.ToByte(false), Convert.ToByte(false) };

                while (true)
                {
                    // poll the server
                    int bytesSent = sender.Send(po_request);

                    // Receive the response from the trackr server.
                    int bytesRec = sender.Receive(bytes);
                    Console.WriteLine("{0}", Encoding.ASCII.GetString(bytes, 0, bytesRec));
                }
                // Release the socket.
                sender.Shutdown(SocketShutdown.Both);
                sender.Close();
                
            } catch (ArgumentNullException ane) {
                Console.WriteLine("ArgumentNullException : {0}",ane.ToString());
            } catch (SocketException se) {
                Console.WriteLine("SocketException : {0}",se.ToString());
            } catch (Exception e) {
                Console.WriteLine("Unexpected exception : {0}", e.ToString());
            }

        } catch (Exception e) {
            Console.WriteLine( e.ToString());
        }
    }

    public static int Main(String[] args)
    {
        StartClient();
        return 0;
    }
}