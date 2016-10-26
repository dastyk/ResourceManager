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
using System.IO.Compression;
using System.Runtime.InteropServices;


namespace Arfer
{

    public partial class Arfer : Form
    {
        private string packOpenedPath = "";
        private string savePath = "";
        private string loadPath = "";
        private UInt64 currentOffset = 0;
        private List<TreeNode> checkedNodes = new List<TreeNode>();
        public Arfer()
        {
            InitializeComponent();
            nodeComp.Text = "Compressed: ";
            nodeExt.Text = "Extension: ";
            nodeSize.Text = "File Size: ";
            fileData.Text = "";
        }
        private void changed()
        {
            saveToolStripMenuItem.Enabled = true;
            saveAsToolStripMenuItem.Enabled = true;
            saveButton.Enabled = true;
            exportToToolStripMenuItem.Enabled = true;

        }
        private void saved()
        {
            saveToolStripMenuItem.Enabled = false;
            saveButton.Enabled = false;
        }
        private void setSelectedNode(TreeNode node)
        {
            itemTree.SelectedNode = node;
            nodeInfoBox.Text = node.Text;
            if (node.Tag != null)
            {
                TreeData data = (TreeData)node.Tag;
                nodeComp.Text = "Compressed: ";
                if (data.compressed != 0 && data.compressed != byte.MaxValue)
                {
                    compRate.Text = "Compression rate: " +  (data.rate*100.0).ToString("F1") + "%";
                    compRate.Visible = true;
                    ucSizeInfo.Text = "Uncompressed size: " + data.csize;
                    ucSizeInfo.Visible = true;
                }
                else
                {
                    ucSizeInfo.Visible = false;
                    compRate.Visible = false;
                }
                if (data.compressed == 1)
                    nodeComp.Text += "LZ77-HUFF.";
                else if(data.compressed == 2)
                    nodeComp.Text += "LZ77.";
                else if(data.compressed == 3)
                    nodeComp.Text += "HUFF-LZ77.";
                else if(data.compressed == 4)
                    nodeComp.Text += "HUFF.";
                else
                    nodeComp.Text += "Not compressed.";
                nodeExt.Text = "Extension: " + data.ext;
                nodeSize.Text = "File Size: " + data.size + " B";
                fileData.Text = data.data;
            }
            else
            {
                nodeComp.Text = "Compressed: ";
                nodeExt.Text = "Extension: ";
                nodeSize.Text = "File Size: ";
                ucSizeInfo.Visible = false;
                compRate.Visible = false;
                fileData.Text = "";
            }
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (saveButton.Enabled)
            {
                DialogResult dialogResult = MessageBox.Show("Save changes?", "Exit", MessageBoxButtons.YesNoCancel);
                if (dialogResult == DialogResult.Yes)
                {

                    if (savePath == "")
                    {
                        SaveFileDialog prom = new SaveFileDialog();
                        prom.Title = "Choose were to save";
                        prom.Filter = "Arfer Package (*.drf)|*.drf|All files (*.*)|*.*";
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
                        saved();
                    }

                }
                else if (dialogResult == DialogResult.No)
                    this.Close();
            }
            else
                this.Close();
        }
        private void Arfer_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (saveButton.Enabled)
            {
                DialogResult dialogResult = MessageBox.Show("Save changes?", "Exit", MessageBoxButtons.YesNoCancel);
                if (dialogResult == DialogResult.Yes)
                {

                    if (savePath == "")
                    {
                        SaveFileDialog prom = new SaveFileDialog();
                        prom.Title = "Choose were to save";
                        prom.Filter = "Arfer Package (*.drf)|*.drf|All files (*.*)|*.*";
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
                        saved();
                    }

                }
                else if (dialogResult == DialogResult.Cancel)

                    e.Cancel = true;
            }

        }
        private void addNodeToNode(TreeNode parent, TreeNode child)
        {
            parent.Nodes.Add(child);
            changed();
        }
        private void addNodeToSelected(ref TreeNode node)
        {
            if (itemTree.Nodes.Count == 0)
            {
                itemTree.Nodes.Add(node);
                changed();
            }
            else
            {
                bool canAdd = true;

                foreach (TreeNode n in itemTree.SelectedNode.Nodes)
                {

                    if (n.Text == node.Text)
                    {
                        if (n.Tag == null && node.Tag == null)
                        {
                            node = n;
                            canAdd = false;
                            break;
                        }
                        else if (n.Tag != null && node.Tag != null)
                        {
                            if (((TreeData)n.Tag).ext == ((TreeData)node.Tag).ext)
                            {
                                node = n;
                                canAdd = false;
                                break;
                            }
                        }
                    }
                }
                if (canAdd)
                {
                    itemTree.SelectedNode.Nodes.Add(node);
                    changed();
                }

            }

        }
        private void removeSelectedNode()
        {
            DialogResult dialogResult = MessageBox.Show("Are you sure you want to remove the node?", "Remove", MessageBoxButtons.YesNo);
            if (dialogResult == DialogResult.Yes)
            {
                if (itemTree.SelectedNode == itemTree.Nodes[0])
                {
                    saved();
                    saveAsToolStripMenuItem.Enabled = false;
                }

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
            node.Tag = null;
            node.Text = "new package";
            node.Name = "proot";
            node.ContextMenuStrip = itemTreeNodeRCCM;


            addNodeToSelected(ref node);
            setSelectedNode(node);
            renameSelectedNode();

            changed();

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
            prom.Filter = "Arfer Package (*.drf)|*.drf|All files (*.*)|*.*";
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
                    checkedNodes.Clear();
                    delChecked.Enabled = false;
                    deleteSelectedToolStripMenuItem.Enabled = false;
                    exportToToolStripMenuItem.Enabled = false;
                    selectedToolStripMenuItem.Enabled = false;
                    selectedToolStripMenuItem1.Enabled = false;
                    using (BinaryReader reader = new BinaryReader(File.Open(prom.FileName, FileMode.Open)))
                    {
                        readFromBinary(reader, itemTree);
                    }
                    
                    packOpenedPath = prom.FileName;
                    savePath = prom.FileName;
                    saved();
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
                prom.Filter = "Arfer Package (*.drf)|*.drf|All files (*.*)|*.*";
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
                saved();
            }
        }

        private void saveAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SaveFileDialog prom = new SaveFileDialog();
            prom.Title = "Choose were to save";
            prom.Filter = "Arfer Package (*.drf)|*.drf|All files (*.*)|*.*";
            prom.DefaultExt = ".drf";
            prom.FileName = itemTree.Nodes[0].Text;
            prom.InitialDirectory = Environment.CurrentDirectory;
            prom.ShowDialog();
            savePath = prom.FileName;


            if (savePath != "")
            {

                writeToBinary(savePath, itemTree);
                packOpenedPath = savePath;
                saved();
            }
        }
        struct LoadData
        {
            public LoadData(string path, UInt64 size, UInt64 offset, UInt64 newOffset, byte comp, string zipP = null)
            {
                this.path = path;
                this.size = size;
                this.offset = offset;
                this.newOffset = newOffset;
                this.zipPath = zipP;
                this.comp = comp;
            }

            public string path;
            public UInt64 size;
            public UInt64 offset;
            public UInt64 newOffset;
            public string zipPath;
            public byte comp;
        }
        private List<LoadData> toLoad = new List<LoadData>();

        private void writeToBinary(string path, TreeView tree)
        {

            using (FileStream fileStream = File.Open("temp", FileMode.Create))
            {

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
                    if (String.IsNullOrEmpty( d.zipPath ))
                    {
                        BinaryReader file;
                        if (d.path == "")
                        {
                            // This file is located in a previous packfile. (At location d.offset back from end of file.)
                            file = new BinaryReader(File.Open(packOpenedPath, FileMode.Open));
                            file.BaseStream.Seek(-Convert.ToInt64( d.offset), SeekOrigin.End);
                        }
                        else
                        {
                            // This file is located in the path.
                            file = new BinaryReader(File.Open(d.path, FileMode.Open));
                        }
                        int bytesRead;
                        long sizeToRead = Convert.ToInt64(d.size);
                        while (sizeToRead > 0 && (bytesRead = file.Read(buffer, 0, (int)Math.Min(buffer.Length, sizeToRead))) > 0)
                        {
                            fileStream.Write(buffer, 0, bytesRead);
                            sizeToRead -= bytesRead;
                        }

                        file.Close();
                        if(d.comp != 0 && !String.IsNullOrEmpty(d.path))
                        {
                            File.Delete(d.path);
                        }
                    }
                    else
                    {
                        using (ZipArchive archive = ZipFile.OpenRead(d.zipPath))
                        {
                            ZipArchiveEntry entry = archive.GetEntry(d.path);
                            if (entry != null)
                            {
                                using (BinaryReader file = new BinaryReader(entry.Open()))
                                {
                                    int bytesRead;
                                    long sizeToRead = Convert.ToInt64(d.size);
                                    while (sizeToRead > 0 && (bytesRead = file.Read(buffer, 0,(int)Math.Min(buffer.Length, sizeToRead))) > 0)
                                    {
                                        fileStream.Write(buffer, 0, bytesRead);
                                        sizeToRead -= bytesRead;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            File.Create(path).Close();
            File.Replace("temp", path, "backup");
            File.Delete("backup");
            File.Delete("temp");

        }
        private void writeToBinary(BinaryWriter writer, TreeNode tree)
        {

            if (tree.Tag == null)
            {
                writer.Write(tree.Text);
                writer.Write(false);
            }
            else
            {
                writer.Write(tree.Text + ((TreeData)tree.Tag).ext);
                writer.Write(true);
                TreeData data = (TreeData)tree.Tag;

                byte com = data.compressed;
                if (data.compressed == byte.MaxValue)
                    data.compressed = 0;

                writer.Write(data.compressed);
                if (data.size != 0)
                {
                    writer.Write(currentOffset + data.size);
                    toLoad.Add(new LoadData(data.filePath, data.size, data.offset, currentOffset + data.size, com, data.zip));
                    data.filePath = "";
                    data.zip = "";
                    data.offset = currentOffset + data.size;
                    currentOffset += data.size;

                }
                writer.Write(data.size);
            }


            writer.Write(tree.Nodes.Count);
            foreach (TreeNode n in tree.Nodes)
            {
                writeToBinary(writer, n);
            }

        }
        private void readFromBinary(BinaryReader reader, TreeView tree)
        {
            TreeNode node = new TreeNode();
            node.Text = reader.ReadString();
            node.ContextMenuStrip = itemTreeNodeRCCM;
            reader.ReadBoolean(); // File or folder (always a folder here)



            int count = reader.ReadInt32();
            node.Tag = null;
            addNodeToSelected(ref node);
            for (int i = 0; i < count; i++)
            {
                readFromBinary(reader, tree.Nodes[0]);
            }



        }
        private void readFromBinary(BinaryReader reader, TreeNode tree)
        {
            TreeNode node = new TreeNode();
           
            node.Text = reader.ReadString();
            if (reader.ReadBoolean())
            {
                        
                TreeData data = new TreeData();
                data.ext = Path.GetExtension(node.Text);
                node.Text = Path.GetFileNameWithoutExtension(node.Text);
                data.compressed = reader.ReadByte();
                                data.filePath = "";
                
                data.offset = reader.ReadUInt64();
                data.size = reader.ReadUInt64();

                node.ContextMenuStrip = itemTreeFileNodeRCCM;
                long headPos = reader.BaseStream.Position;
                reader.BaseStream.Seek(-Convert.ToInt64(data.offset), SeekOrigin.End);
                if (data.compressed > 0)
                {
                    data.csize = reader.ReadUInt64();
                    data.rate = Convert.ToDouble(data.size) / Convert.ToDouble(data.csize);
                    reader.BaseStream.Seek(-sizeof(UInt64), SeekOrigin.Current);
                }
                data.data = System.Text.Encoding.UTF8.GetString(reader.ReadBytes((int)Math.Min(500, data.size)));
                reader.BaseStream.Seek(headPos, SeekOrigin.Begin);

                node.Tag = data;

            }
            else
                node.ContextMenuStrip = itemTreeNodeRCCM;

            int count = reader.ReadInt32();           
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
                            changed();
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
            node.Tag = null;
            addNodeToSelected(ref node);
            setSelectedNode(node);
            renameSelectedNode();


        }
        private void addFile(string path, string zipPath = null, ZipArchiveEntry entry = null)
        {
           
                loadPath = Path.GetDirectoryName(path);
                string fileName = Path.GetFileNameWithoutExtension(path);

                TreeNode node = new TreeNode(fileName);
                node.Name = path;
                node.ContextMenuStrip = itemTreeFileNodeRCCM;
                TreeData data = new TreeData();
                data.offset = 0;
            if (entry == null)
            {
                data.zip = null;
                using (FileStream file = File.Open(path, FileMode.Open))
                {
                    data.size = Convert.ToUInt64(file.Length);
                    BinaryReader br = new BinaryReader(file);
                    data.data = System.Text.Encoding.UTF8.GetString(br.ReadBytes((int)(data.size % 500)));
                }
            }
            else
            {
                data.zip = zipPath;
                data.size = Convert.ToUInt64( entry.Length);
                using (StreamReader file = new StreamReader(entry.Open()))
                {
                    char[] buffer = new char[500];
                    file.ReadBlock(buffer, 0, (int)(data.size % 500));
                    data.data = new string(buffer, 0, (int)(data.size % 500));
                }
            }
                //currentOffset += data.size;
                data.compressed = 0;
                data.filePath = path;
                data.ext = Path.GetExtension(path);
                node.Tag = data;
                addNodeToSelected(ref node);
                setSelectedNode(node);
            
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
        private bool CanAdd(TreeNode node, TreeNode dest, bool f = false)
        {
            if (node == dest)
                return false;
            bool canAdd = true;
            if (f)
            {
                foreach (TreeNode n in dest.Nodes)
                {
                    if (n.Text == node.Text)
                        return false;
                }
            }
            foreach (TreeNode n in node.Nodes)
            {

                if (n == dest)
                {
                    return false;
                }
                canAdd = CanAdd(n, dest);
                if (!canAdd)
                    return false;
            }
            return canAdd;
        }

        private void itemTree_DragDrop(object sender, DragEventArgs e)
        {
            TreeNode NewNode;
            if (e.Data.GetDataPresent("System.Windows.Forms.TreeNode", false))
            {
                Point pt = ((TreeView)sender).PointToClient(new Point(e.X, e.Y));
                TreeNode DestinationNode = ((TreeView)sender).GetNodeAt(pt);

                if (DestinationNode != null && DestinationNode.Tag == null)
                {
                    NewNode = (TreeNode)e.Data.GetData("System.Windows.Forms.TreeNode");


                    if (CanAdd(NewNode, DestinationNode, true))
                    {
                       
                        DestinationNode.Nodes.Add((TreeNode)NewNode.Clone());
                        DestinationNode.Expand();
                        //Remove Original Node
                        NewNode.Remove();
                        changed();
                    }
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
                            checkedNodes.Clear();

                            using (BinaryReader reader = new BinaryReader(File.Open(path, FileMode.Open)))
                            {
                                readFromBinary(reader, itemTree);
                            }
                            packOpenedPath = path;
                            changed();
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
                                data.size = Convert.ToUInt64( file.Length);
                                BinaryReader br = new BinaryReader(file);
                                data.data = System.Text.Encoding.UTF8.GetString(br.ReadBytes((int)data.size % 500));
                                //currentOffset += data.size;
                                data.compressed = 0;
                                data.filePath = path;
                                data.ext = Path.GetExtension(path);
                                node.Tag = data;
                                addNodeToSelected(ref node);
                                itemTree.SelectedNode.Expand();
                            }
                        }

                    }




                }
            }
        }
        private void addFolder(string path,  string zipPath = "", ZipArchiveEntry entry = null)
        {
            string name = Path.GetFileName(path);
            TreeNode node = new TreeNode(name);
            node.Name = name;
            node.ContextMenuStrip = itemTreeNodeRCCM;
            node.Tag = null;
            addNodeToSelected(ref node);
            setSelectedNode(node);
            List<string> dirs = new List<string>(Directory.EnumerateDirectories(path));
            foreach (string sc in dirs)
            {
                addFolder(sc, zipPath, entry);
                setSelectedNode(node);
            }
            List<string> files = new List<string>(Directory.EnumerateFiles(path));
            foreach (string f in files)
            {
                addFile(f, zipPath, entry);
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
        private void addToChecked(TreeNode node)
        {
            node.Checked = true;
            checkedNodes.Add(node);
            foreach(TreeNode n in node.Nodes)
            {
                addToChecked(n);
            }
        }
        private void removeFromChecked(TreeNode node)
        {
            node.Checked = false;
            checkedNodes.Remove(node);
            foreach (TreeNode n in node.Nodes)
            {
                removeFromChecked(n);
            }
        }
        private bool run = true;
        private void itemTree_AfterCheck(object sender, TreeViewEventArgs e)
        {
            if (run)
            {
                run = false;
                if (e.Node.Checked)
                {
                    addToChecked(e.Node);
                }
                else
                {
                    removeFromChecked(e.Node);
                }

                if (checkedNodes.Count == 0)
                {
                    delChecked.Enabled = false;
                    deleteSelectedToolStripMenuItem.Enabled = false;
                    exportToToolStripMenuItem.Enabled = false;
                    selectedToolStripMenuItem.Enabled = false;
                    selectedToolStripMenuItem1.Enabled = false;
                }
                else
                {
                    delChecked.Enabled = true;
                    deleteSelectedToolStripMenuItem.Enabled = true;
                    exportToToolStripMenuItem.Enabled = true;
                    selectedToolStripMenuItem.Enabled = true;
                    selectedToolStripMenuItem1.Enabled = true;
                }
                run = true;
            }
        }
        private void delSel()
        {
            DialogResult dialogResult = MessageBox.Show("Are you sure you want to remove the selected nodes?", "Remove", MessageBoxButtons.YesNo);
            if (dialogResult == DialogResult.Yes)
            {
                bool last = true;
                foreach (TreeNode n in checkedNodes)
                {
                    if (itemTree.SelectedNode == itemTree.Nodes[0])
                    {
                        saved();
                        last = false;
                        saveAsToolStripMenuItem.Enabled = false;
                    }

                    itemTree.Nodes.Remove(n);



                }
                checkedNodes.Clear();
                delChecked.Enabled = false;
                deleteSelectedToolStripMenuItem.Enabled = false;
                if(last)
                    changed();
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
                prom.Filter = "Arfer Package (*.drf)|*.drf|All files (*.*)|*.*";
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
                saved();
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


        private void zipToolStripMenuItem_Click(object sender, EventArgs e)
        {
            // Import
            OpenFileDialog prom = new OpenFileDialog();
            prom.Title = "Choose Archive to load";
            prom.Filter = "Zip Archive (*.zip)|*.zip|All files (*.*)|*.*";
            prom.DefaultExt = ".zip";
            prom.ShowDialog();
            if (File.Exists(prom.FileName))
            {
                DialogResult dialogResult = DialogResult.Yes;
                if (itemTree.Nodes.Count != 0)
                {
                    dialogResult = MessageBox.Show("Are you sure you want to import this archive? (All unsaved changes will be lost)", "Import Zip", MessageBoxButtons.YesNo);

                }
                if (dialogResult == DialogResult.Yes)
                {

                    if (itemTree.Nodes.Count != 0)
                        itemTree.Nodes.Clear();

                    savePath = "";
                    TreeNode node = new TreeNode();
                    node.Tag = null;
                    node.Text = Path.GetFileNameWithoutExtension(prom.FileName);
                    node.Name = "proot";
                    node.ContextMenuStrip = itemTreeNodeRCCM;

                    addNodeToSelected(ref node);
                    setSelectedNode(node);

                   

                    using (ZipArchive archive = ZipFile.OpenRead(prom.FileName))
                    {
                        foreach (ZipArchiveEntry entry in archive.Entries)
                        {
                            setSelectedNode(itemTree.Nodes[0]);
                            string[] directory = entry.FullName.Split('/');
                            for(int i = 0; i < directory.Length -1; i++)
                            {
                                node = new TreeNode(directory[0]);
                                node.Name = directory[0];
                                node.ContextMenuStrip = itemTreeNodeRCCM;
                                node.Tag = null;
                                addNodeToSelected(ref node);
                                setSelectedNode(node);
                            }
                            addFile(entry.FullName, prom.FileName, entry);
                        }
                    }

                    savePath = "";
                    changed();
                }

            }



        }

        private void WriteAllFiles(string path, TreeNode node, ZipArchive zip)
        {
            string tpath = path + node.Text;
            if (node.Nodes.Count == 0)
            {
                if (node.Tag == null)
                {
                    zip.CreateEntry(tpath + "/");
                }
                else
                {
                    
                    TreeData data = (TreeData)node.Tag;
                    tpath += data.ext;
                    if (String.IsNullOrEmpty(data.zip))
                    {
                        if (data.offset == 0)
                        {
                            zip.CreateEntryFromFile(data.filePath, tpath);
                        }
                        else
                        {
                            ZipArchiveEntry entry = zip.CreateEntry(tpath);
                            using (BinaryWriter writer = new BinaryWriter(entry.Open()))
                            {

                                using (FileStream file = File.Open(packOpenedPath, FileMode.Open))
                                {
                                    file.Seek(-Convert.ToInt64(data.offset), SeekOrigin.End);
                                    byte[] buffer = new byte[2048];
                                    int bytesRead;
                                    long sizeToRead = Convert.ToInt64(data.size);
                                    while (sizeToRead > 0 && (bytesRead = file.Read(buffer, 0, (int)Math.Min(buffer.Length, Convert.ToInt32(sizeToRead)))) > 0)
                                    {
                                        writer.Write(buffer, 0, bytesRead);
                                        sizeToRead -= bytesRead;
                                    }
                                }

                            }
                        }
                    }
                    else
                    {
                        using (ZipArchive archive = ZipFile.OpenRead(data.zip))
                        {
                            ZipArchiveEntry entry = archive.GetEntry(tpath);
                            if (entry != null)
                            {
                                using (BinaryReader file = new BinaryReader(entry.Open()))
                                {


                                    ZipArchiveEntry newEntry = zip.CreateEntry(tpath);
                                    using (BinaryWriter writer = new BinaryWriter(newEntry.Open()))
                                    {
                                        byte[] buffer = new byte[2048];
                                        int bytesRead;
                                        long sizeToRead = Convert.ToInt64( data.size);
                                        while (sizeToRead > 0 && (bytesRead = file.Read(buffer, 0, (int)Math.Min(buffer.Length, Convert.ToInt32(sizeToRead)))) > 0)
                                        {
                                            writer.Write(buffer, 0, bytesRead);
                                            sizeToRead -= bytesRead;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

            }
            else
            {
                foreach (TreeNode n in node.Nodes)
                {
                    WriteAllFiles(tpath + "/", n, zip);
                }
            }
        }
        private void zipToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            // Export
            SaveFileDialog prom = new SaveFileDialog();
            prom.Title = "Choose were to export to";
            prom.Filter = "Zip Archive (*.zip)|*.zip|All files (*.*)|*.*";
            prom.DefaultExt = ".zip";
            prom.FileName = itemTree.Nodes[0].Text;
            prom.ShowDialog();

            if (!String.IsNullOrEmpty(prom.FileName))
            {
                using (FileStream zipToOpen = new FileStream(prom.FileName, FileMode.OpenOrCreate))
                {
                    using (ZipArchive archive = new ZipArchive(zipToOpen, ZipArchiveMode.Create))
                    {
                        foreach (TreeNode n in itemTree.Nodes[0].Nodes)
                            WriteAllFiles("", n, archive);
                    }
                }
            }
        }

        private struct RawData
        {
            public UInt64 size;
            public byte[] data;
        }

        private struct CompressedData
        {
            public UInt64 sizeComp;
            public UInt64 sizeUnComp;
            public IntPtr data;
        }
        [DllImport("LZ77Compression.dll")]
        private static extern void CompressLz77([In, MarshalAs(UnmanagedType.LPArray)] byte[] rdata,
            UInt64 size, ref UInt64 sizeCompressed, ref IntPtr cdata);
        [DllImport("LZ77Compression.dll")]
        private static extern void UncompressLz77(
            [MarshalAs(UnmanagedType.LPArray)] byte[] rdata, UInt64 sizeCompressed,
            [In, MarshalAs(UnmanagedType.LPArray)] byte[] cdata);
        [DllImport("LZ77Compression.dll")]
        private static extern void CompressHuffman([In, MarshalAs(UnmanagedType.LPArray)] byte[] rdata,
            UInt64 size, ref UInt64 sizeCompressed, ref IntPtr cdata);
        [DllImport("LZ77Compression.dll")]
        private static extern void UncompressHuffman(
            [MarshalAs(UnmanagedType.LPArray)] byte[] rdata, UInt64 size,
            [In, MarshalAs(UnmanagedType.LPArray)] byte[] cdata);

        private bool compressNode(TreeNode node, byte method)
        {
            TreeData data = (TreeData)node.Tag;
            if (data.compressed != 0 && data.compressed != byte.MaxValue)
                return false;

            byte[] bytes;
            string path = data.filePath;
            if (String.IsNullOrEmpty(data.zip))
            {
                if (data.offset == 0)
                {
                    FileInfo info = new FileInfo(data.filePath);

                    using (BinaryReader file = new BinaryReader(info.OpenRead()))
                    {
                        bytes = file.ReadBytes(Convert.ToInt32(data.size));
                    }



                }
                else
                {
                    using (BinaryReader file = new BinaryReader(File.Open(packOpenedPath, FileMode.Open)))
                    {
                        file.BaseStream.Seek(-Convert.ToInt64(data.offset), SeekOrigin.End);
                        bytes = file.ReadBytes(Convert.ToInt32(data.size));
                        path = node.Text + data.ext;
                    }
                }
            }
            else
            {
                using (ZipArchive archive = ZipFile.OpenRead(data.zip))
                {
                    ZipArchiveEntry entry = archive.GetEntry(data.filePath);
                    using (BinaryReader file = new BinaryReader(entry.Open()))
                    {
                        bytes = file.ReadBytes(Convert.ToInt32(data.size));

                    }
                }
            }


            UInt64 cSize = 0;
            UInt64 size2 = 0;
            double p = 1.0;
            byte[] cData = null;
            UInt64 size = data.size;

            if (method == 1)
            {

                UInt64 lz77cs = 0;
                byte[] lz77data = null;


                double lz77p = compressLZ77(bytes, size, ref lz77data, ref lz77cs);

                UInt64 lz77huffcs = 0;
                byte[] lz77huffdata = null;
                double lz77huffp = compressHuff(lz77data, lz77cs, ref lz77huffdata, ref lz77huffcs);

                p = lz77huffp * lz77p;
                cData = lz77huffdata;
                cSize = lz77huffcs;
                size2 = lz77cs;
            }
            else if (method == 2)
            {
                UInt64 lz77cs = 0;
                byte[] lz77data = null;

                double lz77p = compressLZ77(bytes, size, ref lz77data, ref lz77cs);
                p = lz77p;
                cData = lz77data;
                cSize = lz77cs;
            }
            else if (method == 3)
            {
                UInt64 huffcs = 0;
                byte[] huffdata = null;
                double huffp = compressHuff(bytes, size, ref huffdata, ref huffcs);


                UInt64 hufflz77cs = 0;
                byte[] hufflz77data = null;
                double hufflz77p = compressLZ77(huffdata, huffcs, ref hufflz77data, ref hufflz77cs);

                p = hufflz77p * huffp;
                cData = hufflz77data;
                cSize = hufflz77cs;
                size2 = huffcs;
            }
            else
            {
                UInt64 huffcs = 0;
                byte[] huffdata = null;
                double huffp = compressHuff(bytes, size, ref huffdata, ref huffcs);

                p = huffp;
                cData = huffdata;
                cSize = huffcs;
            }     

            if (data.compressed == byte.MaxValue)
                path = Path.GetFileNameWithoutExtension(path);

            data.filePath = Path.GetFileName(path) + ".smc";
            data.compressed = method;
            data.offset = 0;
            data.csize = data.size;
            data.zip = null;
            data.rate = p;
            if (method == 1 || method == 3)
                data.size = cSize + sizeof(UInt64) * 2;
            else
                data.size = cSize + sizeof(UInt64);
            using (BinaryWriter ofile = new BinaryWriter(File.Create(data.filePath)))
            {

                ofile.Write(data.csize);
                if (method == 1 || method == 3)
                    ofile.Write(size2);
                ofile.Write(cData);

            }


            return true;
        }

        private bool compressNode(TreeNode node)
        {
            TreeData data = (TreeData)node.Tag;
            if (data.compressed != 0 && data.compressed != byte.MaxValue)
                return false;

            byte[] bytes;
            string path = data.filePath;
            if (String.IsNullOrEmpty(data.zip))
            {
                if (data.offset == 0)
                {
                    FileInfo info = new FileInfo(data.filePath);

                    using (BinaryReader file = new BinaryReader(info.OpenRead()))
                    {
                        bytes = file.ReadBytes(Convert.ToInt32(data.size));
                    }



                }
                else
                {
                    using (BinaryReader file = new BinaryReader(File.Open(packOpenedPath, FileMode.Open)))
                    {
                        file.BaseStream.Seek(-Convert.ToInt64(data.offset), SeekOrigin.End);
                        bytes = file.ReadBytes(Convert.ToInt32(data.size));
                        path = node.Text + data.ext;
                    }
                }
            }
            else
            {
                using (ZipArchive archive = ZipFile.OpenRead(data.zip))
                {
                    ZipArchiveEntry entry = archive.GetEntry(data.filePath);
                    using (BinaryReader file = new BinaryReader(entry.Open()))
                    {
                        bytes = file.ReadBytes(Convert.ToInt32(data.size));

                    }
                }
            }


            UInt64 size = data.size;
            UInt64 lz77cs = 0;
            byte[] lz77data = null;
            UInt64 cSize = 0;
            byte[] cData = null;
            double p = 1.0;
            double lz77p = compressLZ77(bytes, size, ref lz77data, ref lz77cs);
            UInt64 size2 = 0;
            byte compressed = 0;
            if (lz77p < 0.9)
            {
                UInt64 lz77huffcs = 0;
                byte[] lz77huffdata = null;
                double lz77huffp = compressHuff(lz77data, lz77cs, ref lz77huffdata, ref lz77huffcs) ;
                if (lz77huffp < 0.9)
                {
                    lz77huffp *= lz77p;
                    if (lz77huffp < lz77p*0.9)
                    {

                        p = lz77huffp;
                        cData = lz77huffdata;
                        cSize = lz77huffcs;
                        compressed = 1;
                        size2 = lz77cs;
                    }
                    else
                    {
                        p = lz77p;
                        cData = lz77data;
                        cSize = lz77cs;
                        compressed = 2;
                    }
                }
                else
                {
                    p = lz77p;
                    cData = lz77data;
                    cSize = lz77cs;
                    compressed = 2;
                }
            }

            UInt64 huffcs = 0;
            byte[] huffdata = null;
            double huffp = compressHuff(bytes, size, ref huffdata, ref huffcs);
            if (huffp < 0.9)
            {
                UInt64 hufflz77cs = 0;
                byte[] hufflz77data = null;
                double hufflz77p = compressLZ77(huffdata, huffcs, ref hufflz77data, ref hufflz77cs) * huffp;
                if (hufflz77p < 0.9)
                {
                    hufflz77p *= huffp;
                    if (hufflz77p < huffp*0.9)
                    {
                        double temp = p;
                        if (compressed == 2)
                            temp *= 0.9;
                        if (hufflz77p < temp)
                        {
                            p = hufflz77p;
                            cData = hufflz77data;
                            cSize = hufflz77cs;
                            compressed = 3;
                            size2 = huffcs;
                        }
                    }
                    else
                    {
                        if (huffp < p)
                        {
                            p = huffp;
                            cData = huffdata;
                            cSize = huffcs;
                            compressed = 4;
                        }
                    }

                }
                else
                {
                    if (huffp < p)
                    {
                        p = huffp;
                        cData = huffdata;
                        cSize = huffcs;
                        compressed = 4;
                    }
                }
            }

            if (p > 0.9)
                return false;

            if (data.compressed == byte.MaxValue)
                path = Path.GetFileNameWithoutExtension(path);

            data.filePath = Path.GetFileName(path) + ".smc";
            data.compressed = compressed;
            data.offset = 0;
            data.csize = data.size;
            data.zip = null;
            data.rate = p;
            if (compressed == 1 || compressed == 3)
                data.size = cSize + sizeof(UInt64) * 2;
            else
                data.size = cSize + sizeof(UInt64);
            using (BinaryWriter ofile = new BinaryWriter(File.Create(data.filePath)))
            {
                
                ofile.Write(data.csize);
                if (compressed == 1 || compressed == 3)
                    ofile.Write(size2);
                ofile.Write(cData);

            }


            return true;
        }
        private bool uncompressNode(TreeNode node)
        {
            TreeData data = (TreeData)node.Tag;
            if (data.compressed == 0 || data.compressed == byte.MaxValue)
                return false;
            byte[] bytes;
            UInt64 rsize = data.size - sizeof(UInt64);
            UInt64 size;
            UInt64 size2 = 0;
            if (data.offset == 0)
            {
                FileInfo info = new FileInfo(data.filePath);

                using (BinaryReader file = new BinaryReader(info.OpenRead()))
                {
                    size = file.ReadUInt64();

                    if (data.compressed == 1 || data.compressed == 3)
                    {
                        size2 = file.ReadUInt64();
                        rsize -= sizeof(UInt64);
                    }

                    bytes = file.ReadBytes(Convert.ToInt32(rsize));
                }
            }
            else
            {
                using (BinaryReader file = new BinaryReader(File.Open(packOpenedPath, FileMode.Open)))
                {
                    file.BaseStream.Seek(-Convert.ToInt64(data.offset), SeekOrigin.End);
                    size = file.ReadUInt64();
                    if (data.compressed == 1 || data.compressed == 3)
                    {
                        size2 = file.ReadUInt64();
                        rsize -= sizeof(UInt64);
                    }

                    bytes = file.ReadBytes(Convert.ToInt32(rsize));
                    data.filePath = node.Text + data.ext;
                }
            }

            byte[] rData = null;
            if(data.compressed == 1)
            {
                // LZ77-huff
                byte[] hData = null;
                uncompressHuff(ref hData, size2, bytes, rsize);
                uncompressLZ77(ref rData, size, hData, size2);
            }
            else if(data.compressed == 2)
            {
                // LZ77
                uncompressLZ77(ref rData, size, bytes, rsize);
            }
            else if(data.compressed == 3)
            {
                // HUFF-LZ77
                byte[] lz77Data = null;             
                uncompressLZ77(ref lz77Data, size2, bytes, rsize);
                uncompressHuff(ref rData, size, lz77Data, size2);
            }
            else
            {
                // HUFF
                uncompressHuff(ref rData, size, bytes, rsize);
            }




            File.Delete(data.filePath);
            data.filePath = Path.GetFileNameWithoutExtension(data.filePath) + ".puc";
            data.compressed = byte.MaxValue;
            data.csize = 0;
            data.size = size;
            using (BinaryWriter ofile = new BinaryWriter(File.Create(data.filePath)))
            {
                ofile.Write(rData);
            }
            return true;
        }


        private double compressLZ77(byte[] data, ulong size,ref byte[] cdata, ref ulong csize)
        {
           
            IntPtr cdataPtr = new IntPtr(0);
            CompressLz77(data, size, ref csize, ref cdataPtr);

            double p = Convert.ToDouble(csize) / Convert.ToDouble(size);
            cdata = new byte[csize];
            if(p < 0.9)
                Marshal.Copy(cdataPtr, cdata, 0, Convert.ToInt32(csize));
            Marshal.FreeHGlobal(cdataPtr);

            return p;

        }
        private double compressHuff(byte[] data, ulong size, ref byte[] cdata, ref ulong csize)
        {
            IntPtr cdataPtr = new IntPtr(0);
            CompressHuffman(data, size, ref csize, ref cdataPtr);

            double p = Convert.ToDouble(csize) / Convert.ToDouble(size);
            cdata = new byte[csize];
            if (p < 0.9)
                Marshal.Copy(cdataPtr, cdata, 0, Convert.ToInt32(csize));
            Marshal.FreeHGlobal(cdataPtr);

            return p;
        }
        private bool uncompressLZ77(ref byte[] data, ulong size, byte[] cdata, ulong cSize)
        {

            data = new byte[size];
            UncompressLz77(data, cSize, cdata);

           
            return true;
        }
        private bool uncompressHuff(ref byte[] data, ulong size, byte[] cdata, ulong cSize)
        {

            data = new byte[size];
            UncompressHuffman(data, size, cdata);


            return true;
        }


        private void uncompressToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            if (uncompressNode(itemTree.SelectedNode))
            {
                setSelectedNode(itemTree.SelectedNode);
                changed();
            }
                
        }
        private void compressAll(TreeNode node, BackgroundWorker worker)
        {
            
            if (node.Nodes.Count == 0)
            {               
                if (node.Tag != null)
                {
                    worker.ReportProgress(progressBar1.Value + 1, node.Text);
                    compressNode(node);
                }
            }
            else
            {
                foreach(TreeNode n in node.Nodes)
                {
                    compressAll(n, worker);
                }
            }
        }
        private void uncompressAll(TreeNode node, BackgroundWorker worker)
        {
            
            if (node.Nodes.Count == 0)
            {
                if (node.Tag != null)
                {
                    worker.ReportProgress(progressBar1.Value + 1, node.Text);
                    uncompressNode(node);
                }
            }
            else
            {
                foreach (TreeNode n in node.Nodes)
                {
                    uncompressAll(n, worker);
                }
            }
        }
        private void allToolStripMenuItem_Click(object sender, EventArgs e)
        {
            progressBar1.Maximum = itemTree.GetNodeCount(true);
            progressBar1.Step = 1;
            progressBar1.Value = 0;
            progLab.Visible = true;
            progressBar1.Visible = true;
            compressWorker.RunWorkerAsync();
        }

        private void selectedToolStripMenuItem_Click(object sender, EventArgs e)
        {
            progressBar1.Maximum = itemTree.GetNodeCount(true);
            progressBar1.Step = 1;
            progressBar1.Value = 0;
            progLab.Visible = true;
            progressBar1.Visible = true;
            selcW.RunWorkerAsync();
        }

        private void allToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            progressBar1.Maximum = itemTree.GetNodeCount(true);
            progressBar1.Step = 1;
            progressBar1.Value = 0;
            progLab.Visible = true;
            progressBar1.Visible = true;
            uncompressWorker.RunWorkerAsync();
        }

        private void selectedToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            progressBar1.Maximum = itemTree.GetNodeCount(true);
            progressBar1.Step = 1;
            progressBar1.Value = 0;
            progLab.Visible = true;
            progressBar1.Visible = true;
            selucW.RunWorkerAsync();
        }

        private void selectiveCompressionToolStripMenuItem_Click(object sender, EventArgs e)
        {
            progressBar1.Maximum = itemTree.GetNodeCount(true);
            progressBar1.Step = 1;
            progressBar1.Value = 0;
            progLab.Visible = true;
            progressBar1.Visible = true;
            compressWorker.RunWorkerAsync();
        }

        private void compressWorker_DoWork(object sender, DoWorkEventArgs e)
        {
            BackgroundWorker backgroundWorker = sender as BackgroundWorker;

           
            compressAll(itemTree.Nodes[0] ,backgroundWorker);
        }

        private void compressWorker_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            progLab.Text = "Compressing " +  (string)e.UserState;
            progressBar1.Value = e.ProgressPercentage;
            changed();
        }

        private void compressWorker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            setSelectedNode(itemTree.SelectedNode);
            progLab.Visible = false;
            progressBar1.Visible = false;
        }

        private void uncompressWorker_DoWork(object sender, DoWorkEventArgs e)
        {
            BackgroundWorker backgroundWorker = sender as BackgroundWorker;


            uncompressAll(itemTree.Nodes[0], backgroundWorker);
        }

        private void uncompressWorker_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            progLab.Text = "Unompressing " + (string)e.UserState;
            progressBar1.Value = e.ProgressPercentage;
            changed();
        }

        private void uncompressWorker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            setSelectedNode(itemTree.SelectedNode);
            progLab.Visible = false;
            progressBar1.Visible = false;
        }

        private void selcW_DoWork(object sender, DoWorkEventArgs e)
        {
            BackgroundWorker backgroundWorker = sender as BackgroundWorker;
            foreach (TreeNode n in checkedNodes)
            {
                 compressAll(n, backgroundWorker);
            }
        }

        private void selcW_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            progLab.Text = "Compressing " + (string)e.UserState;
            progressBar1.Value = e.ProgressPercentage;
            changed();
        }

        private void selcW_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            setSelectedNode(itemTree.SelectedNode);
            progLab.Visible = false;
            progressBar1.Visible = false;
        }

        private void selucW_DoWork(object sender, DoWorkEventArgs e)
        {
            BackgroundWorker backgroundWorker = sender as BackgroundWorker;
            foreach (TreeNode n in checkedNodes)
            {
                uncompressAll(n, backgroundWorker);
            }
        }

        private void selucW_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            progLab.Text = "Uncompressing " + (string)e.UserState;
            progressBar1.Value = e.ProgressPercentage;
            changed();
        }

        private void selucW_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            setSelectedNode(itemTree.SelectedNode);
            progLab.Visible = false;
            progressBar1.Visible = false;
        }

        private void selectiveToolStripMenuItem_Click(object sender, EventArgs e)
        {

            if (compressNode(itemTree.SelectedNode))
            {
                setSelectedNode(itemTree.SelectedNode);
                changed();
            }

        }

        private void lZ77ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if(compressNode(itemTree.SelectedNode, 2))
            {
                setSelectedNode(itemTree.SelectedNode);
                changed();
            }
        }

        private void huffmanToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (compressNode(itemTree.SelectedNode, 4))
            {
                setSelectedNode(itemTree.SelectedNode);
                changed();
            }
        }

        private void lZ77HuffmanToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (compressNode(itemTree.SelectedNode, 1))
            {
                setSelectedNode(itemTree.SelectedNode);
                changed();
            }
        }

        private void huffmanLZ77ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (compressNode(itemTree.SelectedNode, 3))
            {
                setSelectedNode(itemTree.SelectedNode);
                changed();
            }
        }
    }

    class TreeData
    {
        public UInt64 offset;
        public UInt64 size;
        public byte compressed;
        public UInt64 csize;
        public string ext;
        public string filePath;
        public string data;
        public string zip;
        public double rate;
    }
}
