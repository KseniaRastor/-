namespace messenger
{
    partial class Form1
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
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
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            button1 = new Button();
            textBox_forUserName = new TextBox();
            richTextBox_forInputMessage = new RichTextBox();
            richTextBox_forOutputMessage = new RichTextBox();
            checkBox1_WaitMessage = new CheckBox();
            label1 = new Label();
            label2 = new Label();
            label3 = new Label();
            textBox_forUserPassword = new TextBox();
            button2 = new Button();
            textBox_DateOfRecipient = new TextBox();
            checkBox1_BoldText = new CheckBox();
            checkBox1_ItalicText = new CheckBox();
            checkBox1_UnderlineText = new CheckBox();
            richTextBox1 = new RichTextBox();
            label4 = new Label();
            checkBox1_BroadcastMsg = new CheckBox();
            button3 = new Button();
            button4 = new Button();
            button5 = new Button();
            SuspendLayout();
            // 
            // button1
            // 
            button1.Location = new Point(716, 457);
            button1.Name = "button1";
            button1.Size = new Size(110, 32);
            button1.TabIndex = 0;
            button1.Text = "Отправить";
            button1.UseVisualStyleBackColor = true;
            button1.Click += button1_Click;
            // 
            // textBox_forUserName
            // 
            textBox_forUserName.Location = new Point(39, 108);
            textBox_forUserName.Name = "textBox_forUserName";
            textBox_forUserName.Size = new Size(192, 23);
            textBox_forUserName.TabIndex = 1;
            textBox_forUserName.TextChanged += textBox1_TextChanged;
            // 
            // richTextBox_forInputMessage
            // 
            richTextBox_forInputMessage.Location = new Point(278, 438);
            richTextBox_forInputMessage.Name = "richTextBox_forInputMessage";
            richTextBox_forInputMessage.Size = new Size(424, 51);
            richTextBox_forInputMessage.TabIndex = 2;
            richTextBox_forInputMessage.Text = "";
            richTextBox_forInputMessage.TextChanged += richTextBox1_TextChanged;
            // 
            // richTextBox_forOutputMessage
            // 
            richTextBox_forOutputMessage.BackColor = SystemColors.ButtonFace;
            richTextBox_forOutputMessage.Location = new Point(278, 95);
            richTextBox_forOutputMessage.Name = "richTextBox_forOutputMessage";
            richTextBox_forOutputMessage.ReadOnly = true;
            richTextBox_forOutputMessage.Size = new Size(576, 293);
            richTextBox_forOutputMessage.TabIndex = 3;
            richTextBox_forOutputMessage.Text = "";
            richTextBox_forOutputMessage.TextChanged += richTextBox2_TextChanged;
            // 
            // checkBox1_WaitMessage
            // 
            checkBox1_WaitMessage.AutoSize = true;
            checkBox1_WaitMessage.Location = new Point(716, 404);
            checkBox1_WaitMessage.Name = "checkBox1_WaitMessage";
            checkBox1_WaitMessage.Size = new Size(163, 19);
            checkBox1_WaitMessage.TabIndex = 5;
            checkBox1_WaitMessage.Text = "Отложенное сообщение";
            checkBox1_WaitMessage.UseVisualStyleBackColor = true;
            checkBox1_WaitMessage.CheckedChanged += checkBox1_CheckedChanged;
            // 
            // label1
            // 
            label1.AutoSize = true;
            label1.Font = new Font("Segoe UI", 9F);
            label1.Location = new Point(283, 57);
            label1.Name = "label1";
            label1.Size = new Size(39, 15);
            label1.TabIndex = 6;
            label1.Text = "Кому:";
            label1.Click += label1_Click_1;
            // 
            // label2
            // 
            label2.AutoSize = true;
            label2.Location = new Point(39, 90);
            label2.Name = "label2";
            label2.Size = new Size(44, 15);
            label2.TabIndex = 7;
            label2.Text = "Логин:";
            label2.Click += label2_Click;
            // 
            // label3
            // 
            label3.AutoSize = true;
            label3.Location = new Point(39, 146);
            label3.Name = "label3";
            label3.Size = new Size(52, 15);
            label3.TabIndex = 8;
            label3.Text = "Пароль:";
            // 
            // textBox_forUserPassword
            // 
            textBox_forUserPassword.Location = new Point(39, 164);
            textBox_forUserPassword.Name = "textBox_forUserPassword";
            textBox_forUserPassword.PasswordChar = '*';
            textBox_forUserPassword.Size = new Size(192, 23);
            textBox_forUserPassword.TabIndex = 9;
            textBox_forUserPassword.TextChanged += textBox1_TextChanged_1;
            // 
            // button2
            // 
            button2.Location = new Point(60, 249);
            button2.Name = "button2";
            button2.Size = new Size(134, 42);
            button2.TabIndex = 10;
            button2.Text = "Зарегистрироваться";
            button2.UseVisualStyleBackColor = true;
            button2.Click += button2_Click;
            // 
            // textBox_DateOfRecipient
            // 
            textBox_DateOfRecipient.Location = new Point(327, 54);
            textBox_DateOfRecipient.Name = "textBox_DateOfRecipient";
            textBox_DateOfRecipient.Size = new Size(215, 23);
            textBox_DateOfRecipient.TabIndex = 11;
            textBox_DateOfRecipient.TextChanged += textBox_DateOfRecipient_TextChanged;
            // 
            // checkBox1_BoldText
            // 
            checkBox1_BoldText.Appearance = Appearance.Button;
            checkBox1_BoldText.FlatStyle = FlatStyle.Popup;
            checkBox1_BoldText.Font = new Font("Times New Roman", 9F, FontStyle.Bold, GraphicsUnit.Point, 204);
            checkBox1_BoldText.Location = new Point(278, 404);
            checkBox1_BoldText.Name = "checkBox1_BoldText";
            checkBox1_BoldText.Size = new Size(27, 28);
            checkBox1_BoldText.TabIndex = 12;
            checkBox1_BoldText.Text = "B";
            checkBox1_BoldText.TextAlign = ContentAlignment.MiddleCenter;
            checkBox1_BoldText.UseVisualStyleBackColor = true;
            checkBox1_BoldText.CheckedChanged += checkBox1_CheckedChanged_1;
            // 
            // checkBox1_ItalicText
            // 
            checkBox1_ItalicText.Appearance = Appearance.Button;
            checkBox1_ItalicText.FlatStyle = FlatStyle.Popup;
            checkBox1_ItalicText.Font = new Font("Times New Roman", 9F, FontStyle.Italic, GraphicsUnit.Point, 204);
            checkBox1_ItalicText.Location = new Point(311, 404);
            checkBox1_ItalicText.Name = "checkBox1_ItalicText";
            checkBox1_ItalicText.Size = new Size(27, 28);
            checkBox1_ItalicText.TabIndex = 13;
            checkBox1_ItalicText.Text = "I";
            checkBox1_ItalicText.TextAlign = ContentAlignment.MiddleCenter;
            checkBox1_ItalicText.UseVisualStyleBackColor = true;
            checkBox1_ItalicText.CheckedChanged += checkBox1_ItalicText_CheckedChanged;
            // 
            // checkBox1_UnderlineText
            // 
            checkBox1_UnderlineText.Appearance = Appearance.Button;
            checkBox1_UnderlineText.FlatStyle = FlatStyle.Popup;
            checkBox1_UnderlineText.Font = new Font("Times New Roman", 9F, FontStyle.Underline, GraphicsUnit.Point, 204);
            checkBox1_UnderlineText.Location = new Point(344, 404);
            checkBox1_UnderlineText.Name = "checkBox1_UnderlineText";
            checkBox1_UnderlineText.Size = new Size(27, 28);
            checkBox1_UnderlineText.TabIndex = 14;
            checkBox1_UnderlineText.Text = "U";
            checkBox1_UnderlineText.TextAlign = ContentAlignment.MiddleCenter;
            checkBox1_UnderlineText.UseVisualStyleBackColor = true;
            checkBox1_UnderlineText.CheckedChanged += checkBox1_UnderlineText_CheckedChanged;
            // 
            // richTextBox1
            // 
            richTextBox1.BackColor = SystemColors.ButtonFace;
            richTextBox1.Location = new Point(39, 346);
            richTextBox1.Name = "richTextBox1";
            richTextBox1.Size = new Size(192, 178);
            richTextBox1.TabIndex = 16;
            richTextBox1.Text = "";
            richTextBox1.TextChanged += richTextBox1_TextChanged_1;
            // 
            // label4
            // 
            label4.AutoSize = true;
            label4.Location = new Point(39, 328);
            label4.Name = "label4";
            label4.Size = new Size(132, 15);
            label4.TabIndex = 17;
            label4.Text = "Пользователи онлайн:";
            label4.Click += label4_Click;
            // 
            // checkBox1_BroadcastMsg
            // 
            checkBox1_BroadcastMsg.AutoSize = true;
            checkBox1_BroadcastMsg.Location = new Point(716, 429);
            checkBox1_BroadcastMsg.Name = "checkBox1_BroadcastMsg";
            checkBox1_BroadcastMsg.Size = new Size(114, 19);
            checkBox1_BroadcastMsg.TabIndex = 18;
            checkBox1_BroadcastMsg.Text = "Отправить всем";
            checkBox1_BroadcastMsg.UseVisualStyleBackColor = true;
            checkBox1_BroadcastMsg.CheckedChanged += checkBox1_BroadcastMsg_CheckedChanged;
            // 
            // button3
            // 
            button3.Location = new Point(611, 48);
            button3.Name = "button3";
            button3.Size = new Size(215, 32);
            button3.TabIndex = 19;
            button3.Text = "Загрузить историю переписки";
            button3.UseVisualStyleBackColor = true;
            button3.Click += button3_Click;
            // 
            // button4
            // 
            button4.Location = new Point(575, 402);
            button4.Name = "button4";
            button4.Size = new Size(127, 32);
            button4.TabIndex = 20;
            button4.Text = "Отправить файл";
            button4.UseVisualStyleBackColor = true;
            button4.Click += button4_Click;
            // 
            // button5
            // 
            button5.Location = new Point(60, 201);
            button5.Name = "button5";
            button5.Size = new Size(134, 42);
            button5.TabIndex = 21;
            button5.Text = "Войти";
            button5.UseVisualStyleBackColor = true;
            button5.Click += button5_Click;
            // 
            // Form1
            // 
            AutoScaleMode = AutoScaleMode.None;
            ClientSize = new Size(929, 544);
            Controls.Add(button5);
            Controls.Add(button4);
            Controls.Add(button3);
            Controls.Add(checkBox1_BroadcastMsg);
            Controls.Add(label4);
            Controls.Add(richTextBox1);
            Controls.Add(checkBox1_UnderlineText);
            Controls.Add(checkBox1_ItalicText);
            Controls.Add(checkBox1_BoldText);
            Controls.Add(textBox_DateOfRecipient);
            Controls.Add(button2);
            Controls.Add(textBox_forUserPassword);
            Controls.Add(label3);
            Controls.Add(label2);
            Controls.Add(label1);
            Controls.Add(checkBox1_WaitMessage);
            Controls.Add(richTextBox_forOutputMessage);
            Controls.Add(richTextBox_forInputMessage);
            Controls.Add(textBox_forUserName);
            Controls.Add(button1);
            Name = "Form1";
            Text = "Form1";
            Load += Form1_Load;
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private Button button1;
        private TextBox textBox_forUserName;
        private RichTextBox richTextBox_forInputMessage;
        private RichTextBox richTextBox_forOutputMessage;
        private CheckBox checkBox1_WaitMessage;
        private Label label1;
        private Label label2;
        private Label label3;
        private TextBox textBox_forUserPassword;
        private Button button2;
        private TextBox textBox_DateOfRecipient;
        private CheckBox checkBox1_BoldText;
        private CheckBox checkBox1_ItalicText;
        private CheckBox checkBox1_UnderlineText;
        private RichTextBox richTextBox1;
        private Label label4;
        private CheckBox checkBox1_BroadcastMsg;
        private Button button3;
        private Button button4;
        private Button button5;
    }
}
