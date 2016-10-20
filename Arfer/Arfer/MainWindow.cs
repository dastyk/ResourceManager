﻿using System;
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
        private long currentOffset = 0;
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
                if (data.compressed == 1)
                    nodeComp.Text += "LC77.";
                else
                    nodeComp.Text += "Not compressed.";
                nodeExt.Text = "Extension: " + data.ext;
                nodeSize.Text = "File Size: " + data.size;
                fileData.Text = data.data;
            }
            else
            {
                nodeComp.Text = "Compressed: ";
                nodeExt.Text = "Extension: ";
                nodeSize.Text = "File Size: ";
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
            public LoadData(string path, long size, long offset, long newOffset, string zipP = null)
            {
                this.path = path;
                this.size = size;
                this.offset = offset;
                this.newOffset = newOffset;
                this.zipPath = zipP;
            }

            public string path;
            public long size;
            public long offset;
            public long newOffset;
            public string zipPath;
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
                    if (d.zipPath == null)
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
                            fileStream.Write(buffer, 0, bytesRead);
                            sizeToRead -= bytesRead;
                        }

                        file.Close();
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
                                    long sizeToRead = d.size;
                                    while (sizeToRead > 0 && (bytesRead = file.Read(buffer, 0, (int)Math.Min(buffer.Length, d.size))) > 0)
                                    {
                                        fileStream.Write(buffer,0 , bytesRead);
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


                writer.Write(data.compressed);
                if (data.size != 0)
                {
                    writer.Write(currentOffset + data.size);
                   
                    toLoad.Add(new LoadData(data.filePath, data.size, data.offset, currentOffset + data.size, data.zip));
                    data.filePath = "";
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
                
                data.offset = reader.ReadInt64();
                data.size = reader.ReadInt64();

                node.ContextMenuStrip = itemTreeFileNodeRCCM;
                long headPos = reader.BaseStream.Position;
                reader.BaseStream.Seek(-data.offset, SeekOrigin.End);
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
                    data.size = file.Length;
                    BinaryReader br = new BinaryReader(file);
                    data.data = System.Text.Encoding.UTF8.GetString(br.ReadBytes((int)(data.size % 500)));
                }
            }
            else
            {
                data.zip = zipPath;
                data.size = entry.Length;
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

                if (DestinationNode != null)
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
                                data.size = file.Length;
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
                exportToToolStripMenuItem.Enabled = false;
            }
            else
            {
                delChecked.Enabled = true;
                deleteSelectedToolStripMenuItem.Enabled = true;
                exportToToolStripMenuItem.Enabled = true;
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
                                    file.Seek(-data.offset, SeekOrigin.End);
                                    byte[] buffer = new byte[2048];
                                    int bytesRead;
                                    long sizeToRead = data.size;
                                    while (sizeToRead > 0 && (bytesRead = file.Read(buffer, 0, (int)Math.Min(buffer.Length, data.size))) > 0)
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
                                        long sizeToRead = data.size;
                                        while (sizeToRead > 0 && (bytesRead = file.Read(buffer, 0, (int)Math.Min(buffer.Length, data.size))) > 0)
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
            UInt64 size, ref UInt64 sizeCompressed, ref UInt64 sizeUncompressed, ref IntPtr cdata);
        [DllImport("LZ77Compression.dll")]
        private static extern void UncompressLz77(
            ref IntPtr rdata, ref UInt64 size, UInt64 sizeCompressed, UInt64 sizeUncompressed, 
            [In, MarshalAs(UnmanagedType.LPArray)] byte[] cdata);

        private void toolStripButton1_Click(object sender, EventArgs e)
        {
            FileInfo info = new FileInfo("bunny.obj");
        
            using (BinaryReader file = new BinaryReader(File.Open("bunny.obj", FileMode.Open)))
            {
                RawData data = new RawData();
                data.size = Convert.ToUInt64(info.Length);
                data.data = file.ReadBytes(Convert.ToInt32(data.size));

                CompressedData cdata = new CompressedData();
                CompressLz77(data.data, data.size, ref cdata.sizeComp, ref cdata.sizeUnComp, ref cdata.data);
                using (BinaryWriter writer = new BinaryWriter(File.Open("comp.asd", FileMode.OpenOrCreate)))
                {
                    writer.Write(cdata.sizeComp);
                    writer.Write(cdata.sizeUnComp);
                    byte[] myArray = new byte[Convert.ToInt32(cdata.sizeComp)];
                    Marshal.Copy(cdata.data, myArray, 0, Convert.ToInt32(cdata.sizeComp));
                    writer.Write(myArray, 0, Convert.ToInt32(cdata.sizeComp));                                
                }

                using (BinaryReader reader = new BinaryReader(File.Open("comp.asd", FileMode.Open)))
                {
                    CompressedData cdata2 = new CompressedData();
                    RawData data2 = new RawData();
                    cdata2.sizeComp = reader.ReadUInt64();
                    cdata2.sizeUnComp = reader.ReadUInt64();
                    data2.data = reader.ReadBytes(Convert.ToInt32(cdata2.sizeComp));


                    UncompressLz77(ref cdata2.data, ref data2.size, cdata2.sizeComp, cdata2.sizeUnComp, data2.data);
                    byte[] myArray2 = new byte[Convert.ToInt32(data2.size)];
                    Marshal.Copy(cdata2.data, myArray2, 0, Convert.ToInt32(data2.size));
                    char[] st = System.Text.Encoding.UTF8.GetString(myArray2).ToCharArray();
                    int o = myArray2.Length;
                    for (int i = 0; i < data.data.Length; i++)
                    {
                        if (data.data[i] != myArray2[i])
                        {
                            int asd = 0;
                        }
                    }

                }
            }
           

        }

        private void compressToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            TreeNode node = itemTree.SelectedNode;
            TreeData data = (TreeData)node.Tag;

            if(String.IsNullOrEmpty(data.zip))
            {
                if (data.offset == 0)
                {
                    FileInfo info = new FileInfo(data.filePath);

                    using (BinaryReader file = new BinaryReader(info.OpenRead()))
                    {
                        UInt64 size = Convert.ToUInt64(data.size);
                        byte[] bytes = file.ReadBytes(Convert.ToInt32(data.size));

                        UInt64 cSize= 0;
                        UInt64 ucSize = 0 ;
                        IntPtr cdata= new IntPtr(0);
                        CompressLz77(bytes, size, ref cSize, ref ucSize, ref cdata);

                    }
                }
                else
                {

                }
            }
            else
            {

            }
        }
    }

    class TreeData
    {
        public long offset;
        public long size;
        public byte compressed;
        public string ext;
        public string filePath;
        public string data;
        public string zip;
    }
}
