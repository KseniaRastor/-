using Microsoft.VisualBasic;
using Microsoft.VisualBasic.ApplicationServices;
using System;
using System.Collections;
using System.Reflection.Metadata;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using static System.Runtime.InteropServices.JavaScript.JSType;
using static System.Windows.Forms.VisualStyles.VisualStyleElement.StartPanel;
using Timer = System.Windows.Forms.Timer;

namespace messenger
{
    public partial class Form1 : Form
    {
        [DllImport("functions.dll", CallingConvention = CallingConvention.StdCall)]
        internal static extern int myfuncSendMessage(string ProtocolMassage);
        [DllImport("functions.dll", CallingConvention = CallingConvention.StdCall)]
        internal static extern int myfuncSendFile(string filePath, string ProtocolMessage, int SendPosition, int chunkSize);

        [DllImport("functions.dll", CallingConvention = CallingConvention.StdCall)]
        internal static extern int NotifyServerAboutDisconnection(string UserDate);

        [DllImport("functions.dll", CallingConvention = CallingConvention.StdCall)]
        internal static extern int UserRegistration(string UserDate);

        [DllImport("functions.dll", CallingConvention = CallingConvention.StdCall)]
        internal static extern int UserEnter(string UserDate);

        [DllImport("functions.dll", CallingConvention = CallingConvention.StdCall)]
        [return: MarshalAs(UnmanagedType.LPStr)]
        internal static extern string myfuncReceiptMessage();
        private CancellationTokenSource _cancellationTokenSource;

        [DllImport("functions.dll", CallingConvention = CallingConvention.StdCall)]
        [return: MarshalAs(UnmanagedType.LPStr)]
        internal static extern string myfuncOnlineUserList();
        private CancellationTokenSource _cancelTokenSource;

        [DllImport("functions.dll", CallingConvention = CallingConvention.StdCall)]
        [return: MarshalAs(UnmanagedType.LPStr)]
        internal static extern string ReadingOldMessage(string UserLogin);

        //[DllImport("functions.dll", CallingConvention = CallingConvention.StdCall)]
        //[return: MarshalAs(UnmanagedType.LPStr)]
        //internal static extern string myfuncSendFile(string filePath, int SendPosition);

        public Form1()
        {
            InitializeComponent();
            this.Load += Form2_Load;
            this.FormClosing += MainForm_FormClosing;
        }

        public class ProtocolManager                                           //генерация протокола
        {
            public static string myProtocol = string.Empty;
            public static void EmptyProtocol()
            {
                myProtocol = myProtocol.Remove(0);
            }
            public static void ProtocolGeneration(string PartOfProtocol)
            {
                myProtocol = PartOfProtocol + myProtocol;
            }
            public static string GetProtocol()
            {
                return myProtocol;
            }


        }

        private void Form1_Load(object sender, EventArgs e)           //фоновая нить - чтение сообщений с сервера
        {
            _cancellationTokenSource = new CancellationTokenSource();
        }
        private async Task StartReceivingMessagesAsync(CancellationToken token)
        {
            string oldMessage = "";
            try
            {
                while (!token.IsCancellationRequested)
                {
                    string tmpMessage = await Task1Async();
                    if (!Equals(oldMessage, tmpMessage))
                    {
                        UpdateRichTextBox(tmpMessage);
                        oldMessage = tmpMessage;
                    }
                    await Task.Delay(1000);
                }
            }
            catch (TaskCanceledException)
            {

            }
        }
        private async Task<string> Task1Async()
        {
            return await Task.Run(() => myfuncReceiptMessage());
        }

        private char DesignPartOfProtocol()
        {
            if (checkBox1_BoldText.Checked && checkBox1_ItalicText.Checked && checkBox1_UnderlineText.Checked) return '7';
            if (checkBox1_BoldText.Checked && checkBox1_ItalicText.Checked) return '4';
            if (checkBox1_BoldText.Checked && checkBox1_UnderlineText.Checked) return '5';
            if (checkBox1_ItalicText.Checked && checkBox1_UnderlineText.Checked) return '6';
            if (checkBox1_BoldText.Checked) return '1';
            if (checkBox1_ItalicText.Checked) return '2';
            if (checkBox1_UnderlineText.Checked) return '3';
            return '0';
        }
        private void MessageFormatting(string message)
        {
            if (string.IsNullOrEmpty(message))
            {
                return;  // Прерываем выполнение метода, если строка пустая или null
            }

            FontStyle fontStyle = FontStyle.Regular;
            if (message[0] == '1') fontStyle = FontStyle.Bold;
            if (message[0] == '2') fontStyle = FontStyle.Italic;
            if (message[0] == '3') fontStyle = FontStyle.Underline;
            if (message[0] == '4') fontStyle = FontStyle.Bold | FontStyle.Italic;
            if (message[0] == '5') fontStyle = FontStyle.Bold | FontStyle.Underline;
            if (message[0] == '6') fontStyle = FontStyle.Italic | FontStyle.Underline;
            if (message[0] == '7') fontStyle = FontStyle.Bold | FontStyle.Italic | FontStyle.Underline;

            if (richTextBox_forOutputMessage.InvokeRequired)
            {
                richTextBox_forOutputMessage.Invoke(new Action(() => MessageFormatting(message)));
            }
            else
            {
                if (richTextBox_forOutputMessage == null)
                {
                    return;
                }

                Font currentFont = richTextBox_forOutputMessage.Font;
                if (currentFont == null)
                {
                    return;
                }

                richTextBox_forOutputMessage.SelectionFont = new Font(currentFont, fontStyle);
            }
        }

        private void UpdateRichTextBox(string message)
        {
            if (richTextBox_forOutputMessage.InvokeRequired)
            {
                richTextBox_forOutputMessage.Invoke(new Action(() => UpdateRichTextBox(message)));
            }
            else
            {
                if (message.Length > 1)
                {
                    
                    string message_from = message.Substring(1);
                    int colonIndex = message_from.IndexOf(':');
                    string result_from = "";
                    if (colonIndex > 0) { result_from = message_from.Substring(0, colonIndex); }
                    if ((result_from == textBox_DateOfRecipient.Text) || (result_from == textBox_forUserName.Text) || 
                        (message_from == "Ваш файл получен\n"))
                    {

                        // сохранение текущуей позиции
                        int previousSelectionStart = richTextBox_forOutputMessage.SelectionStart;

                        // переход в конец текста
                        richTextBox_forOutputMessage.SelectionStart = richTextBox_forOutputMessage.TextLength;
                        richTextBox_forOutputMessage.SelectionLength = 0; // ничего не выделяем

                        // обновление стиля перед добавлением текста
                        MessageFormatting(message);
                        message = message.Substring(1);

                        // запись текста с учетом установленного стиля
                        DateTime thisDay = DateTime.Now;
                        string time = thisDay.ToString("yyyy.MM.dd HH:mm");
                        message = time + "   " + message;

                        richTextBox_forOutputMessage.SelectedText = message;

                        // восстановление предыдущего положения курсора
                        richTextBox_forOutputMessage.SelectionStart = previousSelectionStart;
                        richTextBox_forOutputMessage.ScrollToCaret();
                    }
                }

            }
        }
        //отмена задачи при закрытии формы
        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            _cancellationTokenSource.Cancel();
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)        //отключение пользователя от сервера
        {
            string UserName;
            UserName = textBox_forUserName.Text;
            NotifyServerAboutDisconnection(UserName);
        }

        private async void Form2_Load(object sender, EventArgs e)           //фоновая нить - вывод онлайн участников
        {
            _cancelTokenSource = new CancellationTokenSource();
            await UpdatingOnlinUserList(_cancelTokenSource.Token);
        }
        private async Task UpdatingOnlinUserList(CancellationToken token)
        {
            string oldMessage = "";
            try
            {
                while (!token.IsCancellationRequested)
                {
                    string tmpMessage = await Task2Async();
                    UpdateRichTextBoxOnlineUser(tmpMessage);
                    await Task.Delay(1000);
                }
            }
            catch (TaskCanceledException)
            {

            }
        }
        private async Task<string> Task2Async()
        {
            return await Task.Run(() => myfuncOnlineUserList());
        }

        private void UpdateRichTextBoxOnlineUser(string message)
        {
            if (richTextBox1.InvokeRequired)
            {
                richTextBox1.Invoke(new Action(() => UpdateRichTextBoxOnlineUser(message)));
            }
            else
            {
                richTextBox1.Text = message;
                richTextBox1.ScrollToCaret();
            }
        }
        //отмена задачи при закрытии формы
        private void Form2_FormClosing(object sender, FormClosingEventArgs e)
        {
            _cancelTokenSource.Cancel();
        }

        private bool InputUser()
        {
            if (textBox_forUserName.Text.Length == 0)                               //проверка на наличие имени пользователя
            {
                Timer MyTimer = new System.Windows.Forms.Timer();
                textBox_forUserName.BackColor = Color.Red;
                MessageBox.Show("Введите имя пользователя!");
                textBox_forUserName.BackColor = Color.White;
                return false;
            }
            else
            {
                if (textBox_forUserPassword.Text.Length == 0)                       //проверка на наличие пароля пользователя
                {
                    Timer MyTimer = new System.Windows.Forms.Timer();
                    textBox_forUserPassword.BackColor = Color.Red;
                    MessageBox.Show("Введите пароль!");
                    textBox_forUserPassword.BackColor = Color.White;
                    return false;
                }
            }
            return true;
        }

        private bool InputRecepient()
        {
            if (textBox_DateOfRecipient.Text.Length == 0 && !checkBox1_BroadcastMsg.Checked)      //проверка на наличие имени получателя
            {
                Timer MyTimer = new System.Windows.Forms.Timer();
                textBox_DateOfRecipient.BackColor = Color.Red;
                MessageBox.Show("Введите имя получателя!");
                textBox_DateOfRecipient.BackColor = Color.White;
                return false;
            }
            return true;
        }

        private async void button2_Click(object sender, EventArgs e)               //регистрация пользователя на сервере
        {
            if (InputUser())
            {
                string UserDate;
                UserDate = textBox_forUserName.Text + " " + textBox_forUserPassword.Text;
                int RezultFuncOfRegistration = UserRegistration(UserDate);
                await StartReceivingMessagesAsync(_cancellationTokenSource.Token);    //запуск канала чтения
            }
        }

        private async void button5_Click(object sender, EventArgs e)            //вход уже зарегистрированного пользователя
        {
            if (InputUser())
            {
                string UserDate;
                UserDate = textBox_forUserName.Text + " " + textBox_forUserPassword.Text;
                int RezultFuncOfRegistration = UserEnter(UserDate);
                await StartReceivingMessagesAsync(_cancellationTokenSource.Token);    //запуск канала чтения
            }
        }

        private void button1_Click(object sender, EventArgs e)                    //отправка сообщения на сервер
        {

            if (InputUser() && (InputRecepient() || checkBox1_BroadcastMsg.Checked))
            {
                string sOutputMessageNew;
                string sOutputMessageOld;
                string DateOfRecepient = textBox_DateOfRecipient.Text;
                char DesignText = DesignPartOfProtocol();

                sOutputMessageOld = richTextBox_forOutputMessage.Text;
                sOutputMessageNew = textBox_forUserName.Text + ": " + richTextBox_forInputMessage.Text + "\r\n";

                ProtocolManager.ProtocolGeneration(sOutputMessageNew);
                ProtocolManager.ProtocolGeneration(DateOfRecepient);
                ProtocolManager.ProtocolGeneration(textBox_forUserName.Text);
                ProtocolManager.ProtocolGeneration("0000");    //кол-во необходимых отправок для передачи целого файла
                ProtocolManager.ProtocolGeneration("0000");        //номер текущего сообщения
                ProtocolManager.ProtocolGeneration(sOutputMessageNew.Length.ToString().PadLeft(4, '0')); //длина msg
                ProtocolManager.ProtocolGeneration(DateOfRecepient.Length.ToString().PadLeft(3, '0')); //длина to;
                ProtocolManager.ProtocolGeneration(textBox_forUserName.Text.Length.ToString().PadLeft(3, '0')); //длина from                   
                ProtocolManager.ProtocolGeneration(DesignText.ToString()); //design
                ProtocolManager.ProtocolGeneration("0"); //type
                ProtocolManager.ProtocolGeneration(checkBox1_WaitMessage.Checked ? "1" : "0"); //отложенное сообщение
                ProtocolManager.ProtocolGeneration(checkBox1_BroadcastMsg.Checked ? "1" : "0"); //широковещательный режим рассылки

                string RezultProtocol = ProtocolManager.GetProtocol();
                int ResultFuncOfSend = myfuncSendMessage(RezultProtocol);

                //обновление поля для следующего сообщения
                if (ResultFuncOfSend == 200)
                {
                    ProtocolManager.EmptyProtocol();
                    string content = richTextBox_forInputMessage.Text;
                    richTextBox_forInputMessage.Text = "";
                }
            }
        }

        private void button4_Click(object sender, EventArgs e)
        {
            if (InputUser() && (InputRecepient() || checkBox1_BroadcastMsg.Checked))
            {
                string DateOfRecepient = textBox_DateOfRecipient.Text;
                char DesignText = DesignPartOfProtocol();

                string filePath = string.Empty;
                using (OpenFileDialog openFileDialog = new OpenFileDialog())
                {
                    openFileDialog.InitialDirectory = "c:\\";
                    openFileDialog.RestoreDirectory = true;

                    if (openFileDialog.ShowDialog() == DialogResult.OK)
                    {
                        filePath = openFileDialog.FileName;
                    }
                }


                if (!string.IsNullOrEmpty(filePath))
                {
                    int SendPosition = 0;
                    System.IO.FileInfo file = new System.IO.FileInfo(filePath);
                    double SizeOfFile = file.Length;
                    string extension = Path.GetExtension(filePath);

                    char type = '0';
                    if (extension == ".txt") type = '1';
                    if (extension == ".bmp") type = '2';
                    if (extension == ".jpg") type = '3';
                    if (extension == ".gif") type = '4';
                    if (extension == ".png") type = '5';

                    using (FileStream inputStream = new FileStream(filePath, FileMode.Open, FileAccess.Read, FileShare.Read))
                    {
                        byte[] byteBuffer = new byte[inputStream.Length];
                        inputStream.Read(byteBuffer, 0, byteBuffer.Length);
                        List<byte> listByteBuffer = new List<byte>(byteBuffer);

                        double sizerezult = listByteBuffer.Count;
                        double NumberOfOperationForSend = Math.Ceiling(sizerezult / 300);
                        double NumberOfOperationForSend_copy = NumberOfOperationForSend;

                        int NumberCurrentMsg = 0;

                        while (NumberOfOperationForSend > 0)
                        {
                            NumberCurrentMsg++;

                            int chunkSize = (SendPosition + 300 <= (int)sizerezult) ? 300 : ((int)sizerezult - SendPosition);
                            int endIndex = SendPosition + chunkSize - 1;
                     
                            ProtocolManager.ProtocolGeneration(DateOfRecepient);
                            ProtocolManager.ProtocolGeneration(textBox_forUserName.Text);
                            ProtocolManager.ProtocolGeneration(NumberOfOperationForSend_copy.ToString().PadLeft(4, '0'));    //кол-во необходимых отправок для передачи целого файла
                            ProtocolManager.ProtocolGeneration(NumberCurrentMsg.ToString().PadLeft(4, '0'));        //номер текущего сообщения
                            ProtocolManager.ProtocolGeneration(chunkSize.ToString().PadLeft(4, '0')); //длина msg
                            ProtocolManager.ProtocolGeneration(DateOfRecepient.Length.ToString().PadLeft(3, '0')); //длина to;
                            ProtocolManager.ProtocolGeneration(textBox_forUserName.Text.Length.ToString().PadLeft(3, '0')); //длина from                   
                            ProtocolManager.ProtocolGeneration(DesignText.ToString());
                            ProtocolManager.ProtocolGeneration(type.ToString()); //type
                            ProtocolManager.ProtocolGeneration(checkBox1_WaitMessage.Checked ? "1" : "0"); //отложенное сообщение
                            ProtocolManager.ProtocolGeneration(checkBox1_BroadcastMsg.Checked ? "1" : "0"); //широковещательный режим рассылки

                            string RezultProtocol = ProtocolManager.GetProtocol();
                            int ResultFuncOfSendFile = myfuncSendFile(filePath, RezultProtocol, SendPosition, chunkSize);
                            if (ResultFuncOfSendFile != 200) MessageBox.Show("Ошибка при отправке файла");

                            SendPosition += chunkSize;
                            NumberOfOperationForSend--;
                            ProtocolManager.EmptyProtocol();
                        }
                    }
                }
            }
        }

        private void button3_Click(object sender, EventArgs e)      //загрузка прошлых сообщений
        {
            string UserLogin = textBox_forUserName.Text;
            string ListOldMessages = ReadingOldMessage(UserLogin);

            string[] ListSplitMessage = ListOldMessages.Split(new[] { '\n' }, StringSplitOptions.RemoveEmptyEntries);
            string messageOutput;

            foreach (var message in ListSplitMessage)
            {
                if (string.IsNullOrWhiteSpace(message)) continue; // Проверка на пустые сообщения

                string message_from = message.Substring(21);
                int colonIndex = message_from.IndexOf(':');
                string result_from = "";
                if (colonIndex > 0) { result_from = message_from.Substring(0, colonIndex); }
                if ((result_from == textBox_DateOfRecipient.Text) || (result_from == textBox_forUserName.Text) ||
                        (message_from == "Ваш файл получен\n"))
                {
                    // сохранение текущей позиции
                    int previousSelectionStart = richTextBox_forOutputMessage.SelectionStart;

                    // переход в конец текста
                    richTextBox_forOutputMessage.SelectionStart = richTextBox_forOutputMessage.TextLength;
                    richTextBox_forOutputMessage.SelectionLength = 0; // Ничего не выделяем

                    // обновление стиля перед добавлением текста
                    MessageFormatting(message);
                    messageOutput = message.Substring(1);
                    richTextBox_forOutputMessage.SelectedText = messageOutput;

                    // восстановление предыдущего положения курсора
                    richTextBox_forOutputMessage.SelectionStart = previousSelectionStart;
                    richTextBox_forOutputMessage.ScrollToCaret();
                }
            }
        }

        private void checkBox1_CheckedChanged_1(object sender, EventArgs e)
        {

        }

        private void checkBox1_ItalicText_CheckedChanged(object sender, EventArgs e)
        {

        }

        private void checkBox1_UnderlineText_CheckedChanged(object sender, EventArgs e)
        {

        }

        private void richTextBox1_TextChanged(object sender, EventArgs e)
        {


        }

        private void richTextBox2_TextChanged(object sender, EventArgs e)
        {

        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {

        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void label1_Click_1(object sender, EventArgs e)
        {

        }

        private void label2_Click(object sender, EventArgs e)
        {

        }

        private void textBox1_TextChanged_1(object sender, EventArgs e)
        {

        }

        private void textBox_DateOfRecipient_TextChanged(object sender, EventArgs e)
        {

        }

        private void label4_Click(object sender, EventArgs e)
        {

        }

        private void checkBox1_BroadcastMsg_CheckedChanged(object sender, EventArgs e)
        {

        }

        private void richTextBox1_TextChanged_1(object sender, EventArgs e)
        {

        }


    }
}
