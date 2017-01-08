using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;

using Gma.System.MouseKeyHook;


namespace HidKbdMTestApp
{
    public partial class Form1 : Form
    {
        private IKeyboardMouseEvents mouseHook;
        private bool mouseActionsSubscribed = false;
        private int mouseStartX = -1;
        private int mouseStartY = -1;

        SerialPort serialPort;
        private string[] ports = SerialPort.GetPortNames();
        private int[] baudrates = {
            9600, 14400, 19200, 38400, 57600, 115200
        };

        public Form1()
        {
            InitializeComponent();

            refreshListOfPorts();

            baudComboBox.DataSource = baudrates;
            baudComboBox.SelectedIndex = Array.IndexOf(baudrates, 57600);

            modesTabControl.SelectTab(1); 
            if (serialPort == null)
            {
                getModeButton.Enabled = false;
                modesTabControl.Enabled = false;
            }
        }

        private void portsComboxBox_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void connectButton_Click(object sender, EventArgs e)
        {
            if (serialPort == null)
            {
                connect();
                return;
            }

            if (serialPort.IsOpen)
            {
                disconnect();
            }
            else
            {
                connect();
            }
        }

        private void getModeButton_Click(object sender, EventArgs e)
        {
            commLogTextBox.Text += "Current Mode:\r\n";
            serialPort.WriteLine("G,M");
        }

        private void refreshListOfPorts()
        {
            ports = SerialPort.GetPortNames();
            portsComboxBox.DataSource = ports;
        }

        private void connect()
        {
            if (serialPort == null)
            {
                serialPort = new SerialPort(
                    portsComboxBox.SelectedItem.ToString(),
                    int.Parse(baudComboBox.SelectedItem.ToString())
                );
                serialPort.DataReceived += new SerialDataReceivedEventHandler(serialPort_dataReceived);
            }
            else
            {
                serialPort.BaudRate = int.Parse(baudComboBox.SelectedItem.ToString());
                serialPort.PortName = portsComboxBox.SelectedItem.ToString();
            }
            serialPort.Open();

            portsComboxBox.Enabled = false;
            baudComboBox.Enabled = false;
            connectButton.Text = "Disconnect";

            getModeButton.Enabled = true;
            modesTabControl.Enabled = true;
        }

        private void disconnect()
        {
            serialPort.Close();
            portsComboxBox.Enabled = true;
            baudComboBox.Enabled = true;
            connectButton.Text = "Connect";

            getModeButton.Enabled = false;
            modesTabControl.Enabled = false;
        }

        private void serialPort_dataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            Console.Write("RX: " + serialPort.ReadExisting());
        }

        private void tabControl_selectedIndexChanged(object sender, EventArgs e)
        {
            string modeTabText = modesTabControl.SelectedTab.Text.ToLower();
            if (modeTabText.Contains("command"))
            {
                commLogTextBox.Text += "Entering command mode\r\n";
                serialPort.WriteLine("@@@");
            }
            else if (modeTabText.Contains("translate"))
            {
                commLogTextBox.Text += "Entering translate mode\r\n";
                serialPort.WriteLine("$$$");
            }
        }

        private void cmd_autoKeyTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                if (cmd_autoKeyTextBox.Text != "")
                {
                    int hexValue = Int32.Parse(cmd_autoKeyTextBox.Text, System.Globalization.NumberStyles.HexNumber);
                    batchSendKey((byte)hexValue);
                    cmd_autoKeyTextBox.Text = "";
                }
            }
        }

        private void trans_autoKeyTextBox_KeyPress(object sender, KeyPressEventArgs e)
        {
            commLogTextBox.Text += e.KeyChar;
            serialPort.Write(e.KeyChar.ToString());
            trans_autoKeyTextBox.Text = "";
        }

        private void trans_pressEnterButton_Click(object sender, EventArgs e)
        {
            int enterKey = 10;
            commLogTextBox.Text += enterKey.ToString();
            byte[] keys = { (byte)enterKey };
            serialPort.Write(keys, 0, 1);
        }

        private void cmd_keyDownButton_Click(object sender, EventArgs e)
        {
            int hexValue = Int32.Parse(cmd_autoKeyTextBox.Text, System.Globalization.NumberStyles.HexNumber);
            pressKey((byte)hexValue);
        }

        private void cmd_keyUpButton_Click(object sender, EventArgs e)
        {
            int hexValue = Int32.Parse(cmd_autoKeyTextBox.Text, System.Globalization.NumberStyles.HexNumber);
            releaseKey((byte)hexValue);
        }

        private void cmd_keyUpdateButton_Click(object sender, EventArgs e)
        {
            sendReport();
        }

        private void batchSendKey(byte key)
        {
            commLogTextBox.Text += "KS[" + key.ToString() + "] ([] contains value in DEC)\r\n";

            byte[] keys = { key };
            serialPort.Write("KS");
            serialPort.Write(keys, 0, 1);
            serialPort.Write("\r\n");
        }
        private void pressKey(byte key)
        {
            commLogTextBox.Text += "KD[" + key.ToString() + "] ([] contains value in DEC)\r\n";

            byte[] keys = { key };
            serialPort.Write("KD");
            serialPort.Write(keys, 0, 1);
            serialPort.Write("\r\n");
        }
        private void releaseKey(byte key)
        {
            commLogTextBox.Text += "KU[" + key.ToString() + "] ([] contains value in DEC)\r\n";

            byte[] keys = { key };
            serialPort.Write("KU");
            serialPort.Write(keys, 0, 1);
            serialPort.Write("\r\n");
        }
        private void sendReport()
        {
            serialPort.WriteLine("KUPDATE");
        }

        private void linkLabel1_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            System.Diagnostics.Process.Start("http://www.usb.org/developers/hidpage/Hut1_12v2.pdf");
        }

        private void cmd_mouse_monitorMouse_Click(object sender, EventArgs e)
        {
            if (!mouseActionsSubscribed)
            {
                mouseHook = Hook.GlobalEvents();
                mouseHook.MouseMoveExt += globalMouseMoveExt;
                cmd_mouse_monitorMouse.Text = "Stop monitoring";
                mouseActionsSubscribed = true;
            }
            else
            {
                mouseActionsSubscribed = false;
                mouseHook.MouseMoveExt -= globalMouseMoveExt;
                cmd_mouse_monitorMouse.Text = "Monitor desktop mouse";
                mouseStartX = -1;
                mouseStartY = -1;
            }
        }

        private void globalMouseMoveExt(object sender, MouseEventArgs e)
        {
            if ((mouseStartX == -1) || (mouseStartY == -1))
            {
                mouseStartX = e.X;
                mouseStartY = e.Y;
                return;
            }
            int dX = e.X - mouseStartX;
            int dY = e.Y - mouseStartY;
            dXLabel.Text = dX.ToString();
            dYLabel.Text = dY.ToString();

            mouseStartX = e.X;
            mouseStartY = e.Y;

            sendMouseReport(0, dX, dY, 0);
        }

        private void sendMouseReport(int status, int dx, int dy, int dz)
        {
            byte[] mouseReport = { (byte)status, (byte)dx, (byte)dy, (byte)dz };
            serialPort.Write("M");
            serialPort.Write(mouseReport, 0, mouseReport.Length);
            serialPort.Write("\r\n");
        }
    }
}
