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
    class TreeData
    {
        public int offset;
        public int size;
        public bool compressed;
        public string ext;
        public string filePath;
    }

    public partial class Arfer : Form
    {
        private string savePath = "";
        private string loadPath = "";
        private int currentOffset = 0;
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

            savePath = "";
            TreeNode node = new TreeNode();
            node.Text = "new package";
            node.Name = "proot";
            node.ContextMenuStrip = itemTreeNodeRCCM;
            itemTree.Nodes.Add(node);

            itemTree.SelectedNode = node;
            renameSelectedNode();

            saveToolStripMenuItem.Enabled = true;
            saveAsToolStripMenuItem.Enabled = true;
            exportToolStripMenuItem.Enabled = true;



        }

        private void setContextMenyStripForNode(TreeNode node)
        {
            node.ContextMenuStrip = itemTreeNodeRCCM;
            foreach (TreeNode n in node.Nodes)
            {
                setContextMenyStripForNode(n);
            }
        }

        private void existingPackageToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog prom = new OpenFileDialog();
            prom.Title = "Choose package to load";
            prom.Filter = "Arfer Package|*.drf.pak";
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
                exportToolStripMenuItem.Enabled = true;

                setContextMenyStripForNode(itemTree.Nodes[0]);
            }

        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (savePath == "")
            {

                SaveFileDialog prom = new SaveFileDialog();
                prom.Title = "Choose were to save";
                prom.Filter = "Arfer Package|*.drf.pak";
                prom.DefaultExt = ".drf.pak";
                prom.FileName = itemTree.Nodes[0].Text;
                prom.InitialDirectory = Environment.CurrentDirectory;
                prom.ShowDialog();
                savePath = prom.FileName;
            }

            if (savePath != "")
            {
                using (Stream file = File.Open(savePath, FileMode.Create))
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
            prom.Filter = "Arfer Package|*.drf.pak";
            prom.DefaultExt = ".drf.pak";
            prom.FileName = itemTree.Nodes[0].Text;
            prom.InitialDirectory = Environment.CurrentDirectory;
            prom.ShowDialog();
            savePath = prom.FileName;


            if (savePath != "")
            {
                using (Stream file = File.Open(savePath, FileMode.Create))
                {
                    BinaryFormatter bf = new BinaryFormatter();
                    bf.Serialize(file, itemTree.Nodes.Cast<TreeNode>().ToList());
                }

            }
        }


        private void writeFromTree(BinaryWriter writer, TreeNode tree)
        {
            tree.Tag = new TreeData();
            TreeData t =  (TreeData)tree.Tag;
            
            writer.Write(tree.Text.Length);
            writer.Write(tree.Text);
            //writer.Write(offset to data in file) // From end of tree structure.
            //writer.Write(lenght of extension)
            //writer.Write(Extention)
            writer.Write(tree.Nodes.Count);
            foreach(TreeNode n in tree.Nodes)
            {
                writeFromTree(writer, n);
            }
           
        }
        private void exportToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SaveFileDialog prom = new SaveFileDialog();
            prom.Title = "Choose were to export to";
            prom.Filter = "Arfer data|*.drf";
            prom.InitialDirectory = Environment.CurrentDirectory;
            prom.ShowDialog();

            if (prom.FileName != "")
            {
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

                        e.Node.Name = e.Label;
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

        private void renameToolStripMenuItem_Click(object sender, EventArgs e)
        {
            renameSelectedNode();

        }

        private void itemTree_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            if (e.Button == MouseButtons.Right) itemTree.SelectedNode = e.Node;
        }


        private void addFolderToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            TreeNode node = itemTree.SelectedNode;
            TreeNode newn = new TreeNode("new folder");
            newn.Name = "new folder";
            newn.ContextMenuStrip = itemTreeNodeRCCM;
            node.Nodes.Add(newn);
            itemTree.SelectedNode = newn;
            renameSelectedNode();


        }

        private void addFileToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog prom = new OpenFileDialog();
            prom.Title = "Choose file to add";
            if (loadPath != "")
                prom.InitialDirectory = loadPath;
            prom.ShowDialog();
            if (File.Exists(prom.FileName))
            {
                loadPath = Path.GetDirectoryName(prom.FileName);
                string fileName = Path.GetFileName(prom.FileName);
                TreeNode node = itemTree.SelectedNode;
                
                TreeNode newn = new TreeNode(fileName);
                newn.Name = prom.FileName;
                newn.ContextMenuStrip = itemTreeFileNodeRCCM;
                node.Nodes.Add(newn);
                itemTree.SelectedNode = newn;
            }
        }

        private void renameToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            renameSelectedNode();
        }

        private void renameSelectedNode()
        {
            itemTree.LabelEdit = true;
            itemTree.SelectedNode.BeginEdit();
        }
        private void removeSelectedNode()
        {
            DialogResult dialogResult = MessageBox.Show("Are you sure you want to remove the node?", "Remove", MessageBoxButtons.YesNo);
            if (dialogResult == DialogResult.Yes)
            {
                if (itemTree.SelectedNode == itemTree.Nodes[0])
                {
                    saveToolStripMenuItem.Enabled = false;
                    saveAsToolStripMenuItem.Enabled = false;
                    exportToolStripMenuItem.Enabled = false;
                }
                itemTree.Nodes.Remove(itemTree.SelectedNode);
            }
           
        }
        private void removeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            removeSelectedNode();
        }

        private void removeToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            removeSelectedNode();
        }

        private void expandAllToolStripMenuItem_Click(object sender, EventArgs e)
        {
            itemTree.ExpandAll();
        }

        private void collapseAllToolStripMenuItem_Click(object sender, EventArgs e)
        {
            itemTree.CollapseAll();
        }

        private void expandAllToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            itemTree.SelectedNode.ExpandAll();
        }

        private void collapseAllToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            itemTree.SelectedNode.Collapse();
        }
    }
}
