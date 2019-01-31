import java.io.*;
import java.net.HttpURLConnection;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.URL;

public class server {
    private int port = 20000;
    private ServerSocket serverSocket;

    public server() throws Exception{
        serverSocket = new ServerSocket(port,3);
        System.out.println("服务器启动!");
    }
    public void service(){
        while(true){
            Socket socket = null;
            try {
                socket = serverSocket.accept();
                System.out.println("New connection accepted "+
                        socket.getInetAddress()+":"+socket.getPort());

                InputStream inputStream=socket.getInputStream();//得到一个输入流，接收客户端传递的信息
                InputStreamReader inputStreamReader;//提高效率，将自己字节流转为字符流
                inputStreamReader = new InputStreamReader(inputStream);
                BufferedReader bufferedReader=new BufferedReader(inputStreamReader);//加入缓冲区
                String temp;
                String info="";

                temp = bufferedReader.readLine();
                while(temp!=null){
                    info+=temp;
                    temp = bufferedReader.readLine();
                }
                System.out.println("服务端接收到客户端信息："+info+",当前客户端ip为："+socket.getInetAddress().getHostAddress());

                String[] strs = info.split(" ");
                if(strs[0].equals("GET")){

                }else{
                    if(strs.length >= 3) {
                        String path = strs[1];
                        String params = strs[2];
                        URL url = new URL("http://127.0.0.1:10000" + path);
                        HttpURLConnection con = (HttpURLConnection) url.openConnection();
                        con.setRequestMethod("POST");
                        con.setRequestProperty("Content-Type", "application/x-www-form-urlencoded");
                        con.setRequestProperty("Content-Length", params.length() + "");
                        con.setDoOutput(true);

                        OutputStream outputStream = null;
                        OutputStreamWriter outputStreamWriter = null;
                        InputStream istream = null;
                        InputStreamReader reader = null;
                        BufferedReader breader = null;
                        StringBuffer resultBuffer = new StringBuffer();
                        String tempLine = null;
                        try {
                            outputStream = con.getOutputStream();
                            outputStreamWriter = new OutputStreamWriter(outputStream);

                            outputStreamWriter.write(params);
                            outputStreamWriter.flush();

                            istream = con.getInputStream();
                            reader = new InputStreamReader(istream);
                            breader = new BufferedReader(reader);

                            while ((tempLine = breader.readLine()) != null) {
                                System.out.print(tempLine);
                            }
                        } catch (Exception e) {
                            e.printStackTrace();
                        } finally {
                            outputStreamWriter.close();
                            outputStream.close();
                        }
                    }
                }

                OutputStream outputStream=socket.getOutputStream();//获取一个输出流，向服务端发送信息
                PrintWriter printWriter=new PrintWriter(outputStream);//将输出流包装成打印流
                printWriter.print("rogger that");
                printWriter.flush();
                socket.shutdownOutput();//关闭输出流

                //关闭相对应的资源
                printWriter.close();
                outputStream.close();
                bufferedReader.close();
                inputStream.close();
            } catch (IOException e) {
                e.printStackTrace();
            } finally{
                if(socket!=null){
                    try {
                        socket.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
        }
    }

    public static void main(String[] args) throws Exception{
        server s = new server();
        s.service();
    }
}
