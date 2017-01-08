namespace HidKbdMTestApp
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.connectButton = new System.Windows.Forms.Button();
            this.portsComboxBox = new System.Windows.Forms.ComboBox();
            this.baudComboBox = new System.Windows.Forms.ComboBox();
            this.getModeButton = new System.Windows.Forms.Button();
            this.currentModeTextbox = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.modesTabControl = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage3 = new System.Windows.Forms.TabPage();
            this.label5 = new System.Windows.Forms.Label();
            this.label17 = new System.Windows.Forms.Label();
            this.linkLabel1 = new System.Windows.Forms.LinkLabel();
            this.label6 = new System.Windows.Forms.Label();
            this.cmd_keyUpdateButton = new System.Windows.Forms.Button();
            this.cmd_keyUpButton = new System.Windows.Forms.Button();
            this.cmd_keyDownButton = new System.Windows.Forms.Button();
            this.label4 = new System.Windows.Forms.Label();
            this.cmd_autoKeyTextBox = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.tabPage4 = new System.Windows.Forms.TabPage();
            this.panel1 = new System.Windows.Forms.Panel();
            this.label12 = new System.Windows.Forms.Label();
            this.dZLabel = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.dYLabel = new System.Windows.Forms.Label();
            this.label13 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.dXLabel = new System.Windows.Forms.Label();
            this.cmd_mouse_monitorMouse = new System.Windows.Forms.Button();
            this.tabPage5 = new System.Windows.Forms.TabPage();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.trans_pressEnterButton = new System.Windows.Forms.Button();
            this.trans_autoKeyTextBox = new System.Windows.Forms.TextBox();
            this.label18 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.commLogTextBox = new System.Windows.Forms.TextBox();
            this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.cmd_settings_devNameButton = new System.Windows.Forms.Button();
            this.cmd_settings_devNameTextBox = new System.Windows.Forms.TextBox();
            this.cmd_settings_resetButton = new System.Windows.Forms.Button();
            this.cmd_settings_disconnectButton = new System.Windows.Forms.Button();
            this.button4 = new System.Windows.Forms.Button();
            this.button5 = new System.Windows.Forms.Button();
            this.modesTabControl.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.tabControl1.SuspendLayout();
            this.tabPage3.SuspendLayout();
            this.tabPage4.SuspendLayout();
            this.panel1.SuspendLayout();
            this.tabPage5.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.SuspendLayout();
            // 
            // connectButton
            // 
            this.connectButton.Location = new System.Drawing.Point(229, 11);
            this.connectButton.Name = "connectButton";
            this.connectButton.Size = new System.Drawing.Size(75, 23);
            this.connectButton.TabIndex = 1;
            this.connectButton.Text = "Connect";
            this.connectButton.UseVisualStyleBackColor = true;
            this.connectButton.Click += new System.EventHandler(this.connectButton_Click);
            // 
            // portsComboxBox
            // 
            this.portsComboxBox.FormattingEnabled = true;
            this.portsComboxBox.Location = new System.Drawing.Point(12, 12);
            this.portsComboxBox.Name = "portsComboxBox";
            this.portsComboxBox.Size = new System.Drawing.Size(121, 21);
            this.portsComboxBox.TabIndex = 2;
            this.portsComboxBox.Text = "COM Port";
            this.portsComboxBox.SelectedIndexChanged += new System.EventHandler(this.portsComboxBox_SelectedIndexChanged);
            // 
            // baudComboBox
            // 
            this.baudComboBox.FormattingEnabled = true;
            this.baudComboBox.Location = new System.Drawing.Point(139, 12);
            this.baudComboBox.Name = "baudComboBox";
            this.baudComboBox.Size = new System.Drawing.Size(84, 21);
            this.baudComboBox.TabIndex = 3;
            this.baudComboBox.Text = "Baudrate";
            // 
            // getModeButton
            // 
            this.getModeButton.Location = new System.Drawing.Point(12, 39);
            this.getModeButton.Name = "getModeButton";
            this.getModeButton.Size = new System.Drawing.Size(75, 23);
            this.getModeButton.TabIndex = 4;
            this.getModeButton.Text = "Get Mode";
            this.getModeButton.UseVisualStyleBackColor = true;
            this.getModeButton.Click += new System.EventHandler(this.getModeButton_Click);
            // 
            // currentModeTextbox
            // 
            this.currentModeTextbox.Enabled = false;
            this.currentModeTextbox.Location = new System.Drawing.Point(93, 41);
            this.currentModeTextbox.Name = "currentModeTextbox";
            this.currentModeTextbox.Size = new System.Drawing.Size(100, 20);
            this.currentModeTextbox.TabIndex = 5;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 77);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(164, 13);
            this.label1.TabIndex = 8;
            this.label1.Text = "Select tab to enter and use mode";
            // 
            // modesTabControl
            // 
            this.modesTabControl.Controls.Add(this.tabPage1);
            this.modesTabControl.Controls.Add(this.tabPage2);
            this.modesTabControl.Location = new System.Drawing.Point(15, 93);
            this.modesTabControl.Name = "modesTabControl";
            this.modesTabControl.SelectedIndex = 0;
            this.modesTabControl.Size = new System.Drawing.Size(302, 284);
            this.modesTabControl.TabIndex = 9;
            this.modesTabControl.SelectedIndexChanged += new System.EventHandler(this.tabControl_selectedIndexChanged);
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.tabControl1);
            this.tabPage1.Location = new System.Drawing.Point(4, 22);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.Size = new System.Drawing.Size(294, 258);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "Command Mode";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPage3);
            this.tabControl1.Controls.Add(this.tabPage4);
            this.tabControl1.Controls.Add(this.tabPage5);
            this.tabControl1.Location = new System.Drawing.Point(0, 0);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(294, 262);
            this.tabControl1.TabIndex = 0;
            // 
            // tabPage3
            // 
            this.tabPage3.Controls.Add(this.label5);
            this.tabPage3.Controls.Add(this.label17);
            this.tabPage3.Controls.Add(this.linkLabel1);
            this.tabPage3.Controls.Add(this.label6);
            this.tabPage3.Controls.Add(this.cmd_keyUpdateButton);
            this.tabPage3.Controls.Add(this.cmd_keyUpButton);
            this.tabPage3.Controls.Add(this.cmd_keyDownButton);
            this.tabPage3.Controls.Add(this.label4);
            this.tabPage3.Controls.Add(this.cmd_autoKeyTextBox);
            this.tabPage3.Controls.Add(this.label3);
            this.tabPage3.Location = new System.Drawing.Point(4, 22);
            this.tabPage3.Name = "tabPage3";
            this.tabPage3.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage3.Size = new System.Drawing.Size(286, 236);
            this.tabPage3.TabIndex = 0;
            this.tabPage3.Text = "Keyboard";
            this.tabPage3.UseVisualStyleBackColor = true;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(12, 74);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(121, 13);
            this.label5.TabIndex = 17;
            this.label5.Text = "(using same field above)";
            // 
            // label17
            // 
            this.label17.AutoSize = true;
            this.label17.Location = new System.Drawing.Point(136, 15);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(116, 13);
            this.label17.TabIndex = 16;
            this.label17.Text = "(press ENTER to send)";
            // 
            // linkLabel1
            // 
            this.linkLabel1.AutoSize = true;
            this.linkLabel1.Location = new System.Drawing.Point(9, 211);
            this.linkLabel1.Name = "linkLabel1";
            this.linkLabel1.Size = new System.Drawing.Size(121, 13);
            this.linkLabel1.TabIndex = 15;
            this.linkLabel1.TabStop = true;
            this.linkLabel1.Text = "HID HUT Table (pg. 53)";
            this.linkLabel1.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkLabel1_LinkClicked);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(6, 198);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(246, 13);
            this.label6.TabIndex = 14;
            this.label6.Text = "Textbox must be entered with hex with left padding";
            // 
            // cmd_keyUpdateButton
            // 
            this.cmd_keyUpdateButton.Location = new System.Drawing.Point(13, 155);
            this.cmd_keyUpdateButton.Name = "cmd_keyUpdateButton";
            this.cmd_keyUpdateButton.Size = new System.Drawing.Size(75, 23);
            this.cmd_keyUpdateButton.TabIndex = 13;
            this.cmd_keyUpdateButton.Text = "Send report";
            this.cmd_keyUpdateButton.UseVisualStyleBackColor = true;
            this.cmd_keyUpdateButton.Click += new System.EventHandler(this.cmd_keyUpdateButton_Click);
            // 
            // cmd_keyUpButton
            // 
            this.cmd_keyUpButton.Location = new System.Drawing.Point(13, 126);
            this.cmd_keyUpButton.Name = "cmd_keyUpButton";
            this.cmd_keyUpButton.Size = new System.Drawing.Size(75, 23);
            this.cmd_keyUpButton.TabIndex = 12;
            this.cmd_keyUpButton.Text = "Key up";
            this.cmd_keyUpButton.UseVisualStyleBackColor = true;
            this.cmd_keyUpButton.Click += new System.EventHandler(this.cmd_keyUpButton_Click);
            // 
            // cmd_keyDownButton
            // 
            this.cmd_keyDownButton.Location = new System.Drawing.Point(13, 97);
            this.cmd_keyDownButton.Name = "cmd_keyDownButton";
            this.cmd_keyDownButton.Size = new System.Drawing.Size(75, 23);
            this.cmd_keyDownButton.TabIndex = 11;
            this.cmd_keyDownButton.Text = "Key down";
            this.cmd_keyDownButton.UseVisualStyleBackColor = true;
            this.cmd_keyDownButton.Click += new System.EventHandler(this.cmd_keyDownButton_Click);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(13, 51);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(42, 13);
            this.label4.TabIndex = 9;
            this.label4.Text = "Manual";
            // 
            // cmd_autoKeyTextBox
            // 
            this.cmd_autoKeyTextBox.Location = new System.Drawing.Point(90, 12);
            this.cmd_autoKeyTextBox.Name = "cmd_autoKeyTextBox";
            this.cmd_autoKeyTextBox.Size = new System.Drawing.Size(44, 20);
            this.cmd_autoKeyTextBox.TabIndex = 1;
            this.cmd_autoKeyTextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.cmd_autoKeyTextBox_KeyDown);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(13, 15);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(71, 13);
            this.label3.TabIndex = 0;
            this.label3.Text = "HID Keycode";
            // 
            // tabPage4
            // 
            this.tabPage4.Controls.Add(this.panel1);
            this.tabPage4.Controls.Add(this.cmd_mouse_monitorMouse);
            this.tabPage4.Location = new System.Drawing.Point(4, 22);
            this.tabPage4.Name = "tabPage4";
            this.tabPage4.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage4.Size = new System.Drawing.Size(286, 236);
            this.tabPage4.TabIndex = 1;
            this.tabPage4.Text = "Mouse";
            this.tabPage4.UseVisualStyleBackColor = true;
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.label12);
            this.panel1.Controls.Add(this.dZLabel);
            this.panel1.Controls.Add(this.label11);
            this.panel1.Controls.Add(this.label7);
            this.panel1.Controls.Add(this.label10);
            this.panel1.Controls.Add(this.dYLabel);
            this.panel1.Controls.Add(this.label13);
            this.panel1.Controls.Add(this.label8);
            this.panel1.Controls.Add(this.label9);
            this.panel1.Controls.Add(this.dXLabel);
            this.panel1.Location = new System.Drawing.Point(30, 52);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(200, 111);
            this.panel1.TabIndex = 13;
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(104, 0);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(41, 13);
            this.label12.TabIndex = 7;
            this.label12.Text = "label12";
            // 
            // dZLabel
            // 
            this.dZLabel.AutoSize = true;
            this.dZLabel.Location = new System.Drawing.Point(103, 68);
            this.dZLabel.Name = "dZLabel";
            this.dZLabel.Size = new System.Drawing.Size(41, 13);
            this.dZLabel.TabIndex = 11;
            this.dZLabel.Text = "label16";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(77, 68);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(20, 13);
            this.label11.TabIndex = 6;
            this.label11.Text = "dZ";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(39, 0);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(58, 13);
            this.label7.TabIndex = 2;
            this.label7.Text = "Left button";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(77, 51);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(20, 13);
            this.label10.TabIndex = 5;
            this.label10.Text = "dY";
            // 
            // dYLabel
            // 
            this.dYLabel.AutoSize = true;
            this.dYLabel.Location = new System.Drawing.Point(103, 51);
            this.dYLabel.Name = "dYLabel";
            this.dYLabel.Size = new System.Drawing.Size(41, 13);
            this.dYLabel.TabIndex = 10;
            this.dYLabel.Text = "label15";
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(104, 17);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(41, 13);
            this.label13.TabIndex = 8;
            this.label13.Text = "label13";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(32, 17);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(65, 13);
            this.label8.TabIndex = 3;
            this.label8.Text = "Right button";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(77, 34);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(20, 13);
            this.label9.TabIndex = 4;
            this.label9.Text = "dX";
            // 
            // dXLabel
            // 
            this.dXLabel.AutoSize = true;
            this.dXLabel.Location = new System.Drawing.Point(103, 34);
            this.dXLabel.Name = "dXLabel";
            this.dXLabel.Size = new System.Drawing.Size(41, 13);
            this.dXLabel.TabIndex = 9;
            this.dXLabel.Text = "label14";
            // 
            // cmd_mouse_monitorMouse
            // 
            this.cmd_mouse_monitorMouse.Location = new System.Drawing.Point(49, 23);
            this.cmd_mouse_monitorMouse.Name = "cmd_mouse_monitorMouse";
            this.cmd_mouse_monitorMouse.Size = new System.Drawing.Size(163, 23);
            this.cmd_mouse_monitorMouse.TabIndex = 0;
            this.cmd_mouse_monitorMouse.Text = "Monitor desktop mouse";
            this.cmd_mouse_monitorMouse.UseVisualStyleBackColor = true;
            this.cmd_mouse_monitorMouse.Click += new System.EventHandler(this.cmd_mouse_monitorMouse_Click);
            // 
            // tabPage5
            // 
            this.tabPage5.Controls.Add(this.button5);
            this.tabPage5.Controls.Add(this.button4);
            this.tabPage5.Controls.Add(this.cmd_settings_disconnectButton);
            this.tabPage5.Controls.Add(this.cmd_settings_resetButton);
            this.tabPage5.Controls.Add(this.cmd_settings_devNameTextBox);
            this.tabPage5.Controls.Add(this.cmd_settings_devNameButton);
            this.tabPage5.Location = new System.Drawing.Point(4, 22);
            this.tabPage5.Name = "tabPage5";
            this.tabPage5.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage5.Size = new System.Drawing.Size(286, 236);
            this.tabPage5.TabIndex = 2;
            this.tabPage5.Text = "Settings";
            this.tabPage5.UseVisualStyleBackColor = true;
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.trans_pressEnterButton);
            this.tabPage2.Controls.Add(this.trans_autoKeyTextBox);
            this.tabPage2.Controls.Add(this.label18);
            this.tabPage2.Location = new System.Drawing.Point(4, 22);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(294, 258);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "Translate Mode";
            this.tabPage2.UseVisualStyleBackColor = true;
            // 
            // trans_pressEnterButton
            // 
            this.trans_pressEnterButton.Location = new System.Drawing.Point(10, 30);
            this.trans_pressEnterButton.Name = "trans_pressEnterButton";
            this.trans_pressEnterButton.Size = new System.Drawing.Size(91, 23);
            this.trans_pressEnterButton.TabIndex = 2;
            this.trans_pressEnterButton.Text = "Press ENTER";
            this.trans_pressEnterButton.UseVisualStyleBackColor = true;
            this.trans_pressEnterButton.Click += new System.EventHandler(this.trans_pressEnterButton_Click);
            // 
            // trans_autoKeyTextBox
            // 
            this.trans_autoKeyTextBox.Location = new System.Drawing.Point(65, 4);
            this.trans_autoKeyTextBox.Name = "trans_autoKeyTextBox";
            this.trans_autoKeyTextBox.Size = new System.Drawing.Size(100, 20);
            this.trans_autoKeyTextBox.TabIndex = 1;
            this.trans_autoKeyTextBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.trans_autoKeyTextBox_KeyPress);
            // 
            // label18
            // 
            this.label18.AutoSize = true;
            this.label18.Location = new System.Drawing.Point(7, 7);
            this.label18.Name = "label18";
            this.label18.Size = new System.Drawing.Size(52, 13);
            this.label18.TabIndex = 0;
            this.label18.Text = "Send key";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(328, 17);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(60, 13);
            this.label2.TabIndex = 10;
            this.label2.Text = "Comm. Log";
            // 
            // commLogTextBox
            // 
            this.commLogTextBox.Location = new System.Drawing.Point(331, 33);
            this.commLogTextBox.MaxLength = 1000000000;
            this.commLogTextBox.Multiline = true;
            this.commLogTextBox.Name = "commLogTextBox";
            this.commLogTextBox.ReadOnly = true;
            this.commLogTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.commLogTextBox.Size = new System.Drawing.Size(194, 340);
            this.commLogTextBox.TabIndex = 11;
            // 
            // contextMenuStrip1
            // 
            this.contextMenuStrip1.Name = "contextMenuStrip1";
            this.contextMenuStrip1.Size = new System.Drawing.Size(61, 4);
            // 
            // cmd_settings_devNameButton
            // 
            this.cmd_settings_devNameButton.Location = new System.Drawing.Point(6, 6);
            this.cmd_settings_devNameButton.Name = "cmd_settings_devNameButton";
            this.cmd_settings_devNameButton.Size = new System.Drawing.Size(75, 23);
            this.cmd_settings_devNameButton.TabIndex = 0;
            this.cmd_settings_devNameButton.Text = "Set name";
            this.cmd_settings_devNameButton.UseVisualStyleBackColor = true;
            this.cmd_settings_devNameButton.Click += new System.EventHandler(this.cmd_settings_devNameButton_Click);
            // 
            // cmd_settings_devNameTextBox
            // 
            this.cmd_settings_devNameTextBox.Location = new System.Drawing.Point(87, 8);
            this.cmd_settings_devNameTextBox.MaxLength = 20;
            this.cmd_settings_devNameTextBox.Name = "cmd_settings_devNameTextBox";
            this.cmd_settings_devNameTextBox.Size = new System.Drawing.Size(138, 20);
            this.cmd_settings_devNameTextBox.TabIndex = 1;
            // 
            // cmd_settings_resetButton
            // 
            this.cmd_settings_resetButton.Location = new System.Drawing.Point(7, 36);
            this.cmd_settings_resetButton.Name = "cmd_settings_resetButton";
            this.cmd_settings_resetButton.Size = new System.Drawing.Size(75, 23);
            this.cmd_settings_resetButton.TabIndex = 2;
            this.cmd_settings_resetButton.Text = "Reset";
            this.cmd_settings_resetButton.UseVisualStyleBackColor = true;
            this.cmd_settings_resetButton.Click += new System.EventHandler(this.cmd_settings_resetButton_Click);
            // 
            // cmd_settings_disconnectButton
            // 
            this.cmd_settings_disconnectButton.Location = new System.Drawing.Point(7, 66);
            this.cmd_settings_disconnectButton.Name = "cmd_settings_disconnectButton";
            this.cmd_settings_disconnectButton.Size = new System.Drawing.Size(128, 23);
            this.cmd_settings_disconnectButton.TabIndex = 3;
            this.cmd_settings_disconnectButton.Text = "Disconnect from host";
            this.cmd_settings_disconnectButton.UseVisualStyleBackColor = true;
            this.cmd_settings_disconnectButton.Click += new System.EventHandler(this.cmd_settings_disconnectButton_Click);
            // 
            // button4
            // 
            this.button4.Location = new System.Drawing.Point(7, 96);
            this.button4.Name = "button4";
            this.button4.Size = new System.Drawing.Size(75, 23);
            this.button4.TabIndex = 4;
            this.button4.Text = "Sleep";
            this.button4.UseVisualStyleBackColor = true;
            // 
            // button5
            // 
            this.button5.Location = new System.Drawing.Point(7, 126);
            this.button5.Name = "button5";
            this.button5.Size = new System.Drawing.Size(75, 23);
            this.button5.TabIndex = 5;
            this.button5.Text = "Wake up";
            this.button5.UseVisualStyleBackColor = true;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(537, 389);
            this.Controls.Add(this.commLogTextBox);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.modesTabControl);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.currentModeTextbox);
            this.Controls.Add(this.getModeButton);
            this.Controls.Add(this.baudComboBox);
            this.Controls.Add(this.portsComboxBox);
            this.Controls.Add(this.connectButton);
            this.Name = "Form1";
            this.Text = "cc254x-hidKbdM Test Program";
            this.modesTabControl.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.tabControl1.ResumeLayout(false);
            this.tabPage3.ResumeLayout(false);
            this.tabPage3.PerformLayout();
            this.tabPage4.ResumeLayout(false);
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.tabPage5.ResumeLayout(false);
            this.tabPage5.PerformLayout();
            this.tabPage2.ResumeLayout(false);
            this.tabPage2.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.Button connectButton;
        private System.Windows.Forms.ComboBox portsComboxBox;
        private System.Windows.Forms.ComboBox baudComboBox;
        private System.Windows.Forms.Button getModeButton;
        private System.Windows.Forms.TextBox currentModeTextbox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TabControl modesTabControl;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox commLogTextBox;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage3;
        private System.Windows.Forms.TabPage tabPage4;
        private System.Windows.Forms.TabPage tabPage5;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox cmd_autoKeyTextBox;
        private System.Windows.Forms.Label dZLabel;
        private System.Windows.Forms.Label dYLabel;
        private System.Windows.Forms.Label dXLabel;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Button cmd_mouse_monitorMouse;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.TextBox trans_autoKeyTextBox;
        private System.Windows.Forms.Label label18;
        private System.Windows.Forms.Button trans_pressEnterButton;
        private System.Windows.Forms.Button cmd_keyUpdateButton;
        private System.Windows.Forms.Button cmd_keyUpButton;
        private System.Windows.Forms.Button cmd_keyDownButton;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.LinkLabel linkLabel1;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.Button button5;
        private System.Windows.Forms.Button button4;
        private System.Windows.Forms.Button cmd_settings_disconnectButton;
        private System.Windows.Forms.Button cmd_settings_resetButton;
        private System.Windows.Forms.TextBox cmd_settings_devNameTextBox;
        private System.Windows.Forms.Button cmd_settings_devNameButton;
    }
}

