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
        private string savePath = "";
        private string loadPath = "";
        private long currentOffset = 0;
        public Arfer()
        {
            InitializeComponent();
        }
        private void setSelectedNode(TreeNode node)
        {
            itemTree.SelectedNode = node;
            nodeInfoBox.Text = node.Text;
            TreeData data = (TreeData)node.Tag;
            nodeComp.Text = "Compressed: " + ((data.compressed) ? "Yes" : "No");
            nodeExt.Text = "Extension: " + data.ext;
            nodeSize.Text = "File Size: " + data.size;
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
            TreeData data = new TreeData();
            data.offset = 0;
            data.size = 0;
            data.compressed = false;
            data.filePath = "asd";
            data.ext = "";
            node.Tag = data;
            node.Text = "new package";
            node.Name = "proot";
            node.ContextMenuStrip = itemTreeNodeRCCM;
            itemTree.Nodes.Add(node);

            setSelectedNode(node);
            renameSelectedNode();

            saveToolStripMenuItem.Enabled = true;
            saveAsToolStripMenuItem.Enabled = true;



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
            prom.Filter = "Arfer Package|*.drf";
            prom.ShowDialog();
            if (File.Exists(prom.FileName))
            {
                DialogResult dialogResult = DialogResult.Yes;
                if (itemTree.Nodes.Count != 0)
                {
                    dialogResult = MessageBox.Show("Are you sure you want to load this package? (All unsaved changes will be lost)", "Remove", MessageBoxButtons.YesNo);
                    
                }
                if (dialogResult == DialogResult.Yes)
                {
                    itemTree.Nodes.Clear();


                    using (BinaryReader reader = new BinaryReader(File.Open(prom.FileName, FileMode.Open)))
                    {
                        readFromBinary(reader, itemTree);
                    }
                    saveToolStripMenuItem.Enabled = true;
                    saveAsToolStripMenuItem.Enabled = true;
                }

            }

        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (savePath == "")
            {
                SaveFileDialog prom = new SaveFileDialog();
                prom.Title = "Choose were to save";
                prom.Filter = "Arfer Package|*.drf";
                prom.DefaultExt = ".drf";
                prom.FileName = itemTree.Nodes[0].Text;
                prom.InitialDirectory = Environment.CurrentDirectory;
                prom.ShowDialog();
                savePath = prom.FileName;
            }

            if (savePath != "")
            {
                using (BinaryWriter writer = new BinaryWriter(File.Open(savePath, FileMode.OpenOrCreate)))
                {
                    writeToBinary(writer, itemTree.Nodes[0]);
                }
            }
        }

        private void saveAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SaveFileDialog prom = new SaveFileDialog();
            prom.Title = "Choose were to save";
            prom.Filter = "Arfer Package|*.drf";
            prom.DefaultExt = ".drf";
            prom.FileName = itemTree.Nodes[0].Text;
            prom.InitialDirectory = Environment.CurrentDirectory;
            prom.ShowDialog();
            savePath = prom.FileName;


            if (savePath != "")
            {
                using (BinaryWriter writer = new BinaryWriter(File.Open(savePath, FileMode.OpenOrCreate)))
                {
                    writeToBinary(writer, itemTree.Nodes[0]);
                }

            }
        }

        private void writeToBinary(BinaryWriter writer, TreeNode tree)
        {
            TreeData data =  (TreeData)tree.Tag;
            
            writer.Write(tree.Text);
            writer.Write(data.ext);
            writer.Write(data.size);
            writer.Write(data.offset);
            writer.Write(tree.Nodes.Count);
            foreach(TreeNode n in tree.Nodes)
            {
                writeToBinary(writer, n);
            }
           
        }
        private void readFromBinary(BinaryReader reader, TreeView tree)
        {
            TreeNode node = new TreeNode();
            TreeData data = new TreeData();
            node.Text = reader.ReadString();
            node.ContextMenuStrip = itemTreeNodeRCCM;        
            data.ext = reader.ReadString();
            data.size = reader.ReadInt64();
            data.offset = reader.ReadInt64();
            int count = reader.ReadInt32();
            node.Tag = data;
            tree.Nodes.Add(node);
            for(int i = 0; i < count; i++)
            {
                readFromBinary(reader, tree.Nodes[0]);
            }
        }
        private void readFromBinary(BinaryReader reader, TreeNode tree)
        {
            TreeNode node = new TreeNode();
            TreeData data = new TreeData();
            node.Text = reader.ReadString();           
            data.ext = reader.ReadString();
            data.size = reader.ReadInt64();
            if (data.size == 0)
                node.ContextMenuStrip = itemTreeNodeRCCM;
            else
                node.ContextMenuStrip = itemTreeFileNodeRCCM;
            data.offset = reader.ReadInt64();
            int count = reader.ReadInt32();
            node.Tag = data;
            tree.Nodes.Add(node);
            for (int i = 0; i < count; i++)
            {
                readFromBinary(reader, tree.Nodes[0]);
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
                        nodeInfoBox.Text = e.Label;
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
            setSelectedNode(e.Node);
        }


        private void addFolderToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            TreeNode node = itemTree.SelectedNode;
            TreeNode newn = new TreeNode("new folder");
            newn.Name = "new folder";
            newn.ContextMenuStrip = itemTreeNodeRCCM;
            TreeData data = new TreeData();
            data.offset = 0;
            data.size = 0;
            data.compressed = false;
            data.filePath = "";
            data.ext = "";
            newn.Tag = data;
            node.Nodes.Add(newn);
            setSelectedNode(newn);
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
                FileStream file = File.Open(prom.FileName, FileMode.Open);
                loadPath = Path.GetDirectoryName(prom.FileName);
                string fileName = Path.GetFileName(prom.FileName);
                TreeNode node = itemTree.SelectedNode;
                
                TreeNode newn = new TreeNode(fileName);
                newn.Name = prom.FileName;
                newn.ContextMenuStrip = itemTreeFileNodeRCCM;
                TreeData data = new TreeData();
                data.offset = currentOffset;
                data.size = file.Length;
                currentOffset += data.size;                
                data.compressed = false;
                data.filePath = prom.FileName;
                data.ext = Path.GetExtension(prom.FileName);
                newn.Tag = data;
                node.Nodes.Add(newn);
                setSelectedNode(newn);
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

    class TreeData
    {
        public long offset;
        public long size;
        public bool compressed;
        public string ext;
        public string filePath;
    }
}
