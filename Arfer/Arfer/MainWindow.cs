using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Runtime.Serialization.Formatters.Binary;

namespace Arfer
{
    public partial class Arfer : Form
    {
        private  string pathChoosen = "";
        public Arfer()
        {
            InitializeComponent();
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }
        
        private void newPackageToolStripMenuItem_Click(object sender, EventArgs e)
        {
            
            if (itemTree.Nodes.Count != 0)
                itemTree.Nodes.Clear();

            pathChoosen = "";
            TreeNode node = new TreeNode();
            node.Text = "new package";
            node.Name = "proot";
            itemTree.Nodes.Add(node);

            itemTree.SelectedNode = node;
            itemTree.LabelEdit = true;
            node.BeginEdit();
            
            saveToolStripMenuItem.Enabled = true;
            saveAsToolStripMenuItem.Enabled = true;

        }
        private void existingPackageToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog prom = new OpenFileDialog();
            prom.Title = "Choose file to add";
            prom.Filter = "Arfer Package|*.drf";
            prom.ShowDialog();
            if (File.Exists(prom.FileName))
            {
                itemTree.Nodes.Clear();
                using (Stream file = File.Open(prom.FileName, FileMode.Open))
                {
                    BinaryFormatter bf = new BinaryFormatter();
                    object obj = bf.Deserialize(file);

                    TreeNode[] nodeList = (obj as IEnumerable<TreeNode>).ToArray();
                    itemTree.Nodes.AddRange(nodeList);
                }

                saveToolStripMenuItem.Enabled = true;
                saveAsToolStripMenuItem.Enabled = true;
            }
           
        }
        
        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if(pathChoosen == "")
            {
                
                SaveFileDialog prom = new SaveFileDialog();
                prom.Title = "Choose were to save";
                prom.Filter = "Arfer Package|*.drf";
                prom.InitialDirectory = Environment.CurrentDirectory;
                prom.ShowDialog();
                pathChoosen = prom.FileName;
            }

            if (pathChoosen != "")
            {
                using (Stream file = File.Open(pathChoosen, FileMode.Create))
                {
                    BinaryFormatter bf = new BinaryFormatter();
                    bf.Serialize(file, itemTree.Nodes.Cast<TreeNode>().ToList());
                }
            }
        }

        private void saveAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SaveFileDialog prom = new SaveFileDialog();
            prom.Title = "Choose were to save";
            prom.Filter = "Arfer Package|*.drf";
            prom.InitialDirectory = Environment.CurrentDirectory;
            prom.ShowDialog();
            pathChoosen = prom.FileName;


            if (pathChoosen != "")
            {
                using (Stream file = File.Open(pathChoosen, FileMode.Create))
                {
                    BinaryFormatter bf = new BinaryFormatter();
                    bf.Serialize(file, itemTree.Nodes.Cast<TreeNode>().ToList());
                }
            }
        }
        private void itemTree_AfterLabelEdit(object sender, NodeLabelEditEventArgs e)
        {
            if (e.Label != null)
            {
                if (e.Label.Length > 0)
                {
                    if (e.Label.IndexOfAny(new char[] { '@', '.', ',', '!' }) == -1)
                    {
                        // Stop editing without canceling the label change.
                        e.Node.EndEdit(false);
                    }
                    else
                    {
                        /* Cancel the label edit action, inform the user, and 
                           place the node in edit mode again. */
                        e.CancelEdit = true;
                        MessageBox.Show("Invalid tree node label.\n" +
                           "The invalid characters are: '@','.', ',', '!'",
                           "Node Label Edit");
                        e.Node.BeginEdit();
                    }
                }
                else
                {
                    /* Cancel the label edit action, inform the user, and 
                       place the node in edit mode again. */
                    e.CancelEdit = true;
                    MessageBox.Show("Invalid tree node label.\nThe label cannot be blank",
                       "Node Label Edit");
                    e.Node.BeginEdit();
                }
            }
        }

     
    }
}
