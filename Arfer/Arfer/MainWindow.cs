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
        private string packOpenedPath = "";
        private string savePath = "";
        private string loadPath = "";
        private long currentOffset = 0;
        private long headerSize = 0;
        private List<TreeNode> checkedNodes = new List<TreeNode>();
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
            fileData.Text = data.data;
        }
        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }
        private void addNodeToNode(TreeNode parent, TreeNode child)
        {
            parent.Nodes.Add(child);
            headerSize += child.Text.Length;
            TreeData data = (TreeData)child.Tag;
            headerSize += data.ext.Length;
            headerSize += 28;
        }
        private void addNodeToSelected(TreeNode node)
        {
            if (itemTree.Nodes.Count == 0)
            {
                itemTree.Nodes.Add(node);
            }
            else
            {
                itemTree.SelectedNode.Nodes.Add(node);
            }

            headerSize += node.Text.Length;
            TreeData data = (TreeData)node.Tag;
            headerSize += data.ext.Length;
            headerSize += 28;
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
                    saveButton.Enabled = false;
                }
                headerSize -= itemTree.SelectedNode.Text.Length;
                TreeData data = (TreeData)itemTree.SelectedNode.Tag;
                headerSize -= data.ext.Length;
                headerSize -= 28;

                checkedNodes.Remove(itemTree.SelectedNode);
                itemTree.Nodes.Remove(itemTree.SelectedNode);

                


            }

        }
        private void newPackage()
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


            addNodeToSelected(node);
            setSelectedNode(node);
            renameSelectedNode();

            saveToolStripMenuItem.Enabled = true;
            saveAsToolStripMenuItem.Enabled = true;
            saveButton.Enabled = true;


        }
        private void newPackageToolStripMenuItem_Click(object sender, EventArgs e)
        {

            newPackage();
        }

        private void setContextMenyStripForNode(TreeNode node)
        {
            node.ContextMenuStrip = itemTreeNodeRCCM;
            foreach (TreeNode n in node.Nodes)
            {
                setContextMenyStripForNode(n);
            }
        }
        private void existingPackage()
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
                    dialogResult = MessageBox.Show("Are you sure you want to load this package? (All unsaved changes will be lost)", "Load package", MessageBoxButtons.YesNo);

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
                    saveButton.Enabled = true;
                    packOpenedPath = prom.FileName;
                }

            }
        }
        private void existingPackageToolStripMenuItem_Click(object sender, EventArgs e)
        {
            existingPackage();

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

                writeToBinary(savePath, itemTree);
                packOpenedPath = savePath;
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

                writeToBinary(savePath, itemTree);
                packOpenedPath = savePath;

            }
        }
        struct LoadData
        {
            public LoadData(string path, long size, long offset, long newOffset)
            {
                this.path = path;
                this.size = size;
                this.offset = offset;
                this.newOffset = newOffset;
            }

            public string path;
            public long size;
            public long offset;
            public long newOffset;
        }
        private List<LoadData> toLoad = new List<LoadData>();

        private void writeToBinary(string path, TreeView tree)
        {
            FileStream fileStream;
            if (path != packOpenedPath)
            {
                fileStream = File.Open(path, FileMode.Create);
            }
            else
            {
                fileStream = File.Open("temp", FileMode.Create);
            }

            BinaryWriter writer = new BinaryWriter(fileStream);

            currentOffset = 0;
            toLoad.Clear();
            // Write potential other info about package.
            // Start writing the nodes.
            writeToBinary(writer, itemTree.Nodes[0]);

            // Write all the files.
            byte[] buffer = new byte[2048];
            toLoad.Reverse();
            foreach (LoadData d in toLoad)
            {
                FileStream file;
                if (d.path == "")
                {
                    // This file is located in a previous packfile. (At location d.offset back from end of file.)
                    file = File.Open(packOpenedPath, FileMode.Open);
                    file.Seek(-d.offset, SeekOrigin.End);
                }
                else
                {
                    // This file is located in the path.
                    file = File.Open(d.path, FileMode.Open);
                }
                int bytesRead;
                long sizeToRead = d.size;
                while (sizeToRead > 0 && (bytesRead = file.Read(buffer, 0, (int)Math.Min(buffer.Length, d.size))) > 0)
                {
                    writer.Write(buffer, 0, bytesRead);
                    sizeToRead -= bytesRead;
                }

                file.Close();
            }

            fileStream.Close();
            if (path == packOpenedPath)
            {
                File.Replace("temp", path, "backup");
                File.Delete("backup");
                File.Delete("temp");
            }
        }
        private void writeToBinary(BinaryWriter writer, TreeNode tree)
        {
            TreeData data = (TreeData)tree.Tag;

            writer.Write(tree.Text);
            writer.Write(Convert.ToInt32(data.compressed));
            writer.Write(data.ext);
            if (data.size != 0)
            {
                writer.Write(currentOffset + data.size);
                toLoad.Add(new LoadData(data.filePath, data.size, data.offset, currentOffset + data.size));
                data.filePath = "";
                data.offset = currentOffset + data.size;
                currentOffset += data.size;

            }
            else
            {
                writer.Write((UInt64)0);
            }
            writer.Write(data.size);
            writer.Write(tree.Nodes.Count);
            foreach (TreeNode n in tree.Nodes)
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
            data.compressed = Convert.ToBoolean(reader.ReadInt32());
            data.filePath = "";
            data.ext = reader.ReadString();
            data.offset = reader.ReadInt64();
            data.size = reader.ReadInt64();
            int count = reader.ReadInt32();
            node.Tag = data;
            addNodeToSelected(node);
            for (int i = 0; i < count; i++)
            {
                readFromBinary(reader, tree.Nodes[0]);
            }



        }
        private void readFromBinary(BinaryReader reader, TreeNode tree)
        {
            TreeNode node = new TreeNode();
            TreeData data = new TreeData();
            node.Text = reader.ReadString();
            data.compressed = Convert.ToBoolean(reader.ReadInt32());
            data.filePath = "";
            data.ext = reader.ReadString();
            data.offset = reader.ReadInt64();
            data.size = reader.ReadInt64();
            if (data.size == 0)
                node.ContextMenuStrip = itemTreeNodeRCCM;
            else
            {
                node.ContextMenuStrip = itemTreeFileNodeRCCM;
                long headPos = reader.BaseStream.Position;
                reader.BaseStream.Seek(-data.offset, SeekOrigin.End);
                data.data = System.Text.Encoding.UTF8.GetString(reader.ReadBytes((int)Math.Min(500, data.size)));
                reader.BaseStream.Seek(headPos, SeekOrigin.Begin);
            }
            int count = reader.ReadInt32();


            node.Tag = data;
            addNodeToNode(tree, node);
            for (int i = 0; i < count; i++)
            {
                readFromBinary(reader, node);
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
                        bool taken = false;
                        if (e.Node.Parent != null)
                        {
                            foreach (TreeNode n in e.Node.Parent.Nodes)
                            {
                                if (e.Label == n.Text)
                                {
                                    taken = true;
                                    break;
                                }
                            }
                        }
                        if (!taken)
                        {
                            // Stop editing without canceling the label change.

                            e.Node.EndEdit(false);
                            nodeInfoBox.Text = e.Label;
                            headerSize -= e.Node.Text.Length;
                            headerSize += e.Label.Length;
                        }
                        else
                        {
                            e.CancelEdit = true;
                            MessageBox.Show("The name " + e.Label + " is already taken.",
                               "Node Label Edit");
                            e.Node.BeginEdit();
                        }
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
            TreeNode node = new TreeNode("new folder");
            node.Name = "new folder";
            node.ContextMenuStrip = itemTreeNodeRCCM;
            TreeData data = new TreeData();
            data.offset = 0;
            data.size = 0;
            data.compressed = false;
            data.filePath = "";
            data.ext = "";
            node.Tag = data;
            addNodeToSelected(node);
            setSelectedNode(node);
            renameSelectedNode();


        }
        private void addFile(string path)
        {
            using (FileStream file = File.Open(path, FileMode.Open))
            {
                loadPath = Path.GetDirectoryName(path);
                string fileName = Path.GetFileNameWithoutExtension(path);

                TreeNode node = new TreeNode(fileName);
                node.Name = path;
                node.ContextMenuStrip = itemTreeFileNodeRCCM;
                TreeData data = new TreeData();
                data.offset = 0;
                data.size = file.Length;
                BinaryReader br = new BinaryReader(file);
                data.data = System.Text.Encoding.UTF8.GetString(br.ReadBytes((int)(data.size % 500)));
                //currentOffset += data.size;
                data.compressed = false;
                data.filePath = path;
                data.ext = Path.GetExtension(path);
                node.Tag = data;
                addNodeToSelected(node);
                setSelectedNode(node);
            }
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
                addFile(prom.FileName);
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

        private void itemTree_ItemDrag(object sender, ItemDragEventArgs e)
        {
            if (e.Item.GetType() == typeof(TreeNode))
            {
                if (((TreeNode)e.Item) != itemTree.Nodes[0])
                    DoDragDrop(e.Item, DragDropEffects.Move);
            }


        }

        private void itemTree_DragDrop(object sender, DragEventArgs e)
        {
            TreeNode NewNode;
            if (e.Data.GetDataPresent("System.Windows.Forms.TreeNode", false))
            {
                Point pt = ((TreeView)sender).PointToClient(new Point(e.X, e.Y));
                TreeNode DestinationNode = ((TreeView)sender).GetNodeAt(pt);

                if (((TreeData)DestinationNode.Tag).ext == "")
                {
                    NewNode = (TreeNode)e.Data.GetData("System.Windows.Forms.TreeNode");

                    DestinationNode.Nodes.Add((TreeNode)NewNode.Clone());
                    DestinationNode.Expand();
                    //Remove Original Node
                    NewNode.Remove();
                }


            }
            else if (e.Data.GetDataPresent(DataFormats.FileDrop))
            {
                Point pt = ((TreeView)sender).PointToClient(new Point(e.X, e.Y));
                TreeNode DestinationNode = ((TreeView)sender).GetNodeAt(pt);

                string[] files = (string[])e.Data.GetData(DataFormats.FileDrop);
                foreach (string path in files)
                {
                    if (Path.GetExtension(path) == ".drf")
                    {
                        DialogResult dialogResult = DialogResult.Yes;
                        if (itemTree.Nodes.Count != 0)
                        {
                            dialogResult = MessageBox.Show("Are you sure you want to load this package? (All unsaved changes will be lost)", "Load package", MessageBoxButtons.YesNo);

                        }
                        if (dialogResult == DialogResult.Yes)
                        {
                            itemTree.Nodes.Clear();


                            using (BinaryReader reader = new BinaryReader(File.Open(path, FileMode.Open)))
                            {
                                readFromBinary(reader, itemTree);
                            }
                            saveToolStripMenuItem.Enabled = true;
                            saveAsToolStripMenuItem.Enabled = true;
                            saveButton.Enabled = true;
                            packOpenedPath = path;
                        }
                    }
                    else if (itemTree.Nodes.Count > 0)
                    {

                        if (DestinationNode == null)
                        {
                            DestinationNode = itemTree.Nodes[0];
                        }
                        FileAttributes attr = File.GetAttributes(path);
                        if ((attr & FileAttributes.Directory) == FileAttributes.Directory)
                        {
                            addFolder(path);
                        }
                        else
                        {
                            using (FileStream file = File.Open(path, FileMode.Open))
                            {
                                loadPath = Path.GetDirectoryName(path);
                                string fileName = Path.GetFileNameWithoutExtension(path);

                                TreeNode node = new TreeNode(fileName);
                                node.Name = path;
                                node.ContextMenuStrip = itemTreeFileNodeRCCM;
                                TreeData data = new TreeData();
                                data.offset = 0;
                                data.size = file.Length;
                                BinaryReader br = new BinaryReader(file);
                                data.data = System.Text.Encoding.UTF8.GetString(br.ReadBytes((int)data.size % 500));
                                //currentOffset += data.size;
                                data.compressed = false;
                                data.filePath = path;
                                data.ext = Path.GetExtension(path);
                                node.Tag = data;
                                addNodeToSelected(node);
                                itemTree.SelectedNode.Expand();
                            }
                        }

                    }




                }
            }
        }
        private void addFolder(string path)
        {
            string name = Path.GetFileName(path);
            TreeNode node = new TreeNode(name);
            node.Name = name;
            node.ContextMenuStrip = itemTreeNodeRCCM;
            TreeData data = new TreeData();
            data.offset = 0;
            data.size = 0;
            data.compressed = false;
            data.filePath = "";
            data.ext = "";
            node.Tag = data;
            addNodeToSelected(node);
            setSelectedNode(node);
            List<string> dirs = new List<string>(Directory.EnumerateDirectories(path));
            foreach (string sc in dirs)
            {
                addFolder(sc);
                setSelectedNode(node);
            }
            List<string> files = new List<string>(Directory.EnumerateFiles(path));
            foreach (string f in files)
            {
                addFile(f);
                setSelectedNode(node);
            }

        }

        private void itemTree_DragEnter(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop))
                e.Effect = DragDropEffects.Copy;
            else
                e.Effect = DragDropEffects.Move;
        }

        private void itemTree_DragOver(object sender, DragEventArgs e)
        {

            Point pt = ((TreeView)sender).PointToClient(new Point(e.X, e.Y));
            TreeNode DestinationNode = ((TreeView)sender).GetNodeAt(pt);
            if (DestinationNode != null && DestinationNode != itemTree.SelectedNode)
                setSelectedNode(DestinationNode);

        }

        private void toggleCheckboxesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            itemTree.CheckBoxes = (itemTree.CheckBoxes) ? false : true;
            checkedNodes.Clear();
        }
        private void itemTree_AfterCheck(object sender, TreeViewEventArgs e)
        {
            if (e.Node.Checked)
            {
                checkedNodes.Add(e.Node);
            }
            else
            {
                checkedNodes.Remove(e.Node);
            }
            if(checkedNodes.Count == 0)
            {
                delChecked.Enabled = false;
                deleteSelectedToolStripMenuItem.Enabled = false;
            }
            else
            {
                delChecked.Enabled = true;
                deleteSelectedToolStripMenuItem.Enabled = true;
            }
        }
        private void delSel()
        {
            DialogResult dialogResult = MessageBox.Show("Are you sure you want to remove the selected nodes?", "Remove", MessageBoxButtons.YesNo);
            if (dialogResult == DialogResult.Yes)
            {
                foreach (TreeNode n in checkedNodes)
                {
                    if (itemTree.SelectedNode == itemTree.Nodes[0])
                    {
                        saveToolStripMenuItem.Enabled = false;
                        saveAsToolStripMenuItem.Enabled = false;
                        saveButton.Enabled = false;
                    }

                    headerSize -= itemTree.SelectedNode.Text.Length;
                    TreeData data = (TreeData)itemTree.SelectedNode.Tag;
                    headerSize -= data.ext.Length;
                    headerSize -= 28;
                    itemTree.Nodes.Remove(n);



                }
                checkedNodes.Clear();
                delChecked.Enabled = false;
                deleteSelectedToolStripMenuItem.Enabled = false;
            }
        }
        private void deleteSelectedToolStripMenuItem_Click(object sender, EventArgs e)
        {
            delSel();


        }

        private void checkBoxT_Click(object sender, EventArgs e)
        {
            itemTree.CheckBoxes = (itemTree.CheckBoxes) ? false : true;
            checkedNodes.Clear();
        }

        private void delChecked_Click(object sender, EventArgs e)
        {
            delSel();
        }

        private void saveButton_Click(object sender, EventArgs e)
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

                writeToBinary(savePath, itemTree);
                packOpenedPath = savePath;
            }
        }

        private void newPackButton_Click(object sender, EventArgs e)
        {
            newPackage();
        }

        private void loadButton_Click(object sender, EventArgs e)
        {
            existingPackage();
        }
    }

    class TreeData
    {
        public long offset;
        public long size;
        public bool compressed;
        public string ext;
        public string filePath;
        public string data;
    }
}
