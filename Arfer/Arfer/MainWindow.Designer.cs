﻿namespace Arfer
{
    partial class Arfer
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Arfer));
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.newPackageToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.existingPackageToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripSeparator();
            this.saveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveAsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem2 = new System.Windows.Forms.ToolStripSeparator();
            this.importFromToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.zipToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.exportToToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.zipToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem6 = new System.Windows.Forms.ToolStripSeparator();
            this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.treeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.expandAllToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.collapseAllToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem5 = new System.Windows.Forms.ToolStripSeparator();
            this.toggleCheckboxesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.deleteSelectedToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.itemTree = new System.Windows.Forms.TreeView();
            this.itemTreeRCCM = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.addFolderToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.itemTreeNodeRCCM = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.addFileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.addFolderToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem3 = new System.Windows.Forms.ToolStripSeparator();
            this.renameToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.removeToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem4 = new System.Windows.Forms.ToolStripSeparator();
            this.expandAllToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.collapseAllToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.itemTreeFileNodeRCCM = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.renameToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.removeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.nodeInfoBox = new System.Windows.Forms.GroupBox();
            this.fileData = new System.Windows.Forms.Label();
            this.nodeExt = new System.Windows.Forms.Label();
            this.nodeComp = new System.Windows.Forms.Label();
            this.nodeSize = new System.Windows.Forms.Label();
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.newPackButton = new System.Windows.Forms.ToolStripButton();
            this.saveButton = new System.Windows.Forms.ToolStripButton();
            this.loadButton = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.checkBoxT = new System.Windows.Forms.ToolStripButton();
            this.delChecked = new System.Windows.Forms.ToolStripButton();
            this.toolStripButton1 = new System.Windows.Forms.ToolStripButton();
            this.toolsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.compressToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.allToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.selectedToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.uncompressToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.allToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.selectedToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.selectiveCompressionToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem7 = new System.Windows.Forms.ToolStripSeparator();
            this.compressToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.menuStrip1.SuspendLayout();
            this.itemTreeNodeRCCM.SuspendLayout();
            this.itemTreeFileNodeRCCM.SuspendLayout();
            this.nodeInfoBox.SuspendLayout();
            this.toolStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newPackageToolStripMenuItem,
            this.existingPackageToolStripMenuItem,
            this.toolStripMenuItem1,
            this.saveToolStripMenuItem,
            this.saveAsToolStripMenuItem,
            this.toolStripMenuItem2,
            this.importFromToolStripMenuItem,
            this.exportToToolStripMenuItem,
            this.toolStripMenuItem6,
            this.exitToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(37, 24);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // newPackageToolStripMenuItem
            // 
            this.newPackageToolStripMenuItem.Name = "newPackageToolStripMenuItem";
            this.newPackageToolStripMenuItem.Size = new System.Drawing.Size(150, 22);
            this.newPackageToolStripMenuItem.Text = "New Package";
            this.newPackageToolStripMenuItem.Click += new System.EventHandler(this.newPackageToolStripMenuItem_Click);
            // 
            // existingPackageToolStripMenuItem
            // 
            this.existingPackageToolStripMenuItem.Name = "existingPackageToolStripMenuItem";
            this.existingPackageToolStripMenuItem.Size = new System.Drawing.Size(150, 22);
            this.existingPackageToolStripMenuItem.Text = "Load Package";
            this.existingPackageToolStripMenuItem.Click += new System.EventHandler(this.existingPackageToolStripMenuItem_Click);
            // 
            // toolStripMenuItem1
            // 
            this.toolStripMenuItem1.Name = "toolStripMenuItem1";
            this.toolStripMenuItem1.Size = new System.Drawing.Size(147, 6);
            // 
            // saveToolStripMenuItem
            // 
            this.saveToolStripMenuItem.Enabled = false;
            this.saveToolStripMenuItem.Name = "saveToolStripMenuItem";
            this.saveToolStripMenuItem.Size = new System.Drawing.Size(150, 22);
            this.saveToolStripMenuItem.Text = "Save";
            this.saveToolStripMenuItem.Click += new System.EventHandler(this.saveToolStripMenuItem_Click);
            // 
            // saveAsToolStripMenuItem
            // 
            this.saveAsToolStripMenuItem.Enabled = false;
            this.saveAsToolStripMenuItem.Name = "saveAsToolStripMenuItem";
            this.saveAsToolStripMenuItem.Size = new System.Drawing.Size(150, 22);
            this.saveAsToolStripMenuItem.Text = "Save As";
            this.saveAsToolStripMenuItem.Click += new System.EventHandler(this.saveAsToolStripMenuItem_Click);
            // 
            // toolStripMenuItem2
            // 
            this.toolStripMenuItem2.Name = "toolStripMenuItem2";
            this.toolStripMenuItem2.Size = new System.Drawing.Size(147, 6);
            // 
            // importFromToolStripMenuItem
            // 
            this.importFromToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.zipToolStripMenuItem});
            this.importFromToolStripMenuItem.Name = "importFromToolStripMenuItem";
            this.importFromToolStripMenuItem.Size = new System.Drawing.Size(150, 22);
            this.importFromToolStripMenuItem.Text = "Import From...";
            // 
            // zipToolStripMenuItem
            // 
            this.zipToolStripMenuItem.Name = "zipToolStripMenuItem";
            this.zipToolStripMenuItem.Size = new System.Drawing.Size(91, 22);
            this.zipToolStripMenuItem.Text = "Zip";
            this.zipToolStripMenuItem.Click += new System.EventHandler(this.zipToolStripMenuItem_Click);
            // 
            // exportToToolStripMenuItem
            // 
            this.exportToToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.zipToolStripMenuItem1});
            this.exportToToolStripMenuItem.Enabled = false;
            this.exportToToolStripMenuItem.Name = "exportToToolStripMenuItem";
            this.exportToToolStripMenuItem.Size = new System.Drawing.Size(150, 22);
            this.exportToToolStripMenuItem.Text = "Export To...";
            // 
            // zipToolStripMenuItem1
            // 
            this.zipToolStripMenuItem1.Name = "zipToolStripMenuItem1";
            this.zipToolStripMenuItem1.Size = new System.Drawing.Size(91, 22);
            this.zipToolStripMenuItem1.Text = "Zip";
            this.zipToolStripMenuItem1.Click += new System.EventHandler(this.zipToolStripMenuItem1_Click);
            // 
            // toolStripMenuItem6
            // 
            this.toolStripMenuItem6.Name = "toolStripMenuItem6";
            this.toolStripMenuItem6.Size = new System.Drawing.Size(147, 6);
            // 
            // exitToolStripMenuItem
            // 
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            this.exitToolStripMenuItem.Size = new System.Drawing.Size(150, 22);
            this.exitToolStripMenuItem.Text = "Exit";
            this.exitToolStripMenuItem.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
            // 
            // menuStrip1
            // 
            this.menuStrip1.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.treeToolStripMenuItem,
            this.toolsToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Padding = new System.Windows.Forms.Padding(0);
            this.menuStrip1.Size = new System.Drawing.Size(613, 24);
            this.menuStrip1.TabIndex = 0;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // treeToolStripMenuItem
            // 
            this.treeToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.expandAllToolStripMenuItem,
            this.collapseAllToolStripMenuItem,
            this.toolStripMenuItem5,
            this.toggleCheckboxesToolStripMenuItem,
            this.deleteSelectedToolStripMenuItem});
            this.treeToolStripMenuItem.Name = "treeToolStripMenuItem";
            this.treeToolStripMenuItem.Size = new System.Drawing.Size(41, 24);
            this.treeToolStripMenuItem.Text = "Tree";
            // 
            // expandAllToolStripMenuItem
            // 
            this.expandAllToolStripMenuItem.Name = "expandAllToolStripMenuItem";
            this.expandAllToolStripMenuItem.Size = new System.Drawing.Size(176, 22);
            this.expandAllToolStripMenuItem.Text = "Expand All";
            this.expandAllToolStripMenuItem.Click += new System.EventHandler(this.expandAllToolStripMenuItem_Click);
            // 
            // collapseAllToolStripMenuItem
            // 
            this.collapseAllToolStripMenuItem.Name = "collapseAllToolStripMenuItem";
            this.collapseAllToolStripMenuItem.Size = new System.Drawing.Size(176, 22);
            this.collapseAllToolStripMenuItem.Text = "Collapse All";
            this.collapseAllToolStripMenuItem.Click += new System.EventHandler(this.collapseAllToolStripMenuItem_Click);
            // 
            // toolStripMenuItem5
            // 
            this.toolStripMenuItem5.Name = "toolStripMenuItem5";
            this.toolStripMenuItem5.Size = new System.Drawing.Size(173, 6);
            // 
            // toggleCheckboxesToolStripMenuItem
            // 
            this.toggleCheckboxesToolStripMenuItem.Name = "toggleCheckboxesToolStripMenuItem";
            this.toggleCheckboxesToolStripMenuItem.Size = new System.Drawing.Size(176, 22);
            this.toggleCheckboxesToolStripMenuItem.Text = "Toggle Checkboxes";
            this.toggleCheckboxesToolStripMenuItem.Click += new System.EventHandler(this.toggleCheckboxesToolStripMenuItem_Click);
            // 
            // deleteSelectedToolStripMenuItem
            // 
            this.deleteSelectedToolStripMenuItem.Enabled = false;
            this.deleteSelectedToolStripMenuItem.Name = "deleteSelectedToolStripMenuItem";
            this.deleteSelectedToolStripMenuItem.Size = new System.Drawing.Size(176, 22);
            this.deleteSelectedToolStripMenuItem.Text = "Delete Selected";
            this.deleteSelectedToolStripMenuItem.Click += new System.EventHandler(this.deleteSelectedToolStripMenuItem_Click);
            // 
            // itemTree
            // 
            this.itemTree.AllowDrop = true;
            this.itemTree.ContextMenuStrip = this.itemTreeRCCM;
            this.itemTree.Location = new System.Drawing.Point(0, 52);
            this.itemTree.Name = "itemTree";
            this.itemTree.Size = new System.Drawing.Size(310, 508);
            this.itemTree.TabIndex = 1;
            this.itemTree.AfterLabelEdit += new System.Windows.Forms.NodeLabelEditEventHandler(this.itemTree_AfterLabelEdit);
            this.itemTree.AfterCheck += new System.Windows.Forms.TreeViewEventHandler(this.itemTree_AfterCheck);
            this.itemTree.ItemDrag += new System.Windows.Forms.ItemDragEventHandler(this.itemTree_ItemDrag);
            this.itemTree.NodeMouseClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.itemTree_NodeMouseClick);
            this.itemTree.DragDrop += new System.Windows.Forms.DragEventHandler(this.itemTree_DragDrop);
            this.itemTree.DragEnter += new System.Windows.Forms.DragEventHandler(this.itemTree_DragEnter);
            this.itemTree.DragOver += new System.Windows.Forms.DragEventHandler(this.itemTree_DragOver);
            // 
            // itemTreeRCCM
            // 
            this.itemTreeRCCM.Name = "itemTreeRCCM";
            this.itemTreeRCCM.Size = new System.Drawing.Size(61, 4);
            // 
            // addFolderToolStripMenuItem
            // 
            this.addFolderToolStripMenuItem.Name = "addFolderToolStripMenuItem";
            this.addFolderToolStripMenuItem.Size = new System.Drawing.Size(32, 19);
            // 
            // itemTreeNodeRCCM
            // 
            this.itemTreeNodeRCCM.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addFileToolStripMenuItem,
            this.addFolderToolStripMenuItem1,
            this.toolStripMenuItem3,
            this.renameToolStripMenuItem,
            this.removeToolStripMenuItem1,
            this.toolStripMenuItem4,
            this.expandAllToolStripMenuItem1,
            this.collapseAllToolStripMenuItem1});
            this.itemTreeNodeRCCM.Name = "itemTreeNodeRCCM";
            this.itemTreeNodeRCCM.Size = new System.Drawing.Size(137, 148);
            // 
            // addFileToolStripMenuItem
            // 
            this.addFileToolStripMenuItem.Name = "addFileToolStripMenuItem";
            this.addFileToolStripMenuItem.Size = new System.Drawing.Size(136, 22);
            this.addFileToolStripMenuItem.Text = "Add File";
            this.addFileToolStripMenuItem.Click += new System.EventHandler(this.addFileToolStripMenuItem_Click);
            // 
            // addFolderToolStripMenuItem1
            // 
            this.addFolderToolStripMenuItem1.Name = "addFolderToolStripMenuItem1";
            this.addFolderToolStripMenuItem1.Size = new System.Drawing.Size(136, 22);
            this.addFolderToolStripMenuItem1.Text = "Add Folder";
            this.addFolderToolStripMenuItem1.Click += new System.EventHandler(this.addFolderToolStripMenuItem1_Click);
            // 
            // toolStripMenuItem3
            // 
            this.toolStripMenuItem3.Name = "toolStripMenuItem3";
            this.toolStripMenuItem3.Size = new System.Drawing.Size(133, 6);
            // 
            // renameToolStripMenuItem
            // 
            this.renameToolStripMenuItem.Name = "renameToolStripMenuItem";
            this.renameToolStripMenuItem.Size = new System.Drawing.Size(136, 22);
            this.renameToolStripMenuItem.Text = "Rename";
            this.renameToolStripMenuItem.Click += new System.EventHandler(this.renameToolStripMenuItem_Click);
            // 
            // removeToolStripMenuItem1
            // 
            this.removeToolStripMenuItem1.Name = "removeToolStripMenuItem1";
            this.removeToolStripMenuItem1.Size = new System.Drawing.Size(136, 22);
            this.removeToolStripMenuItem1.Text = "Remove";
            this.removeToolStripMenuItem1.Click += new System.EventHandler(this.removeToolStripMenuItem1_Click);
            // 
            // toolStripMenuItem4
            // 
            this.toolStripMenuItem4.Name = "toolStripMenuItem4";
            this.toolStripMenuItem4.Size = new System.Drawing.Size(133, 6);
            // 
            // expandAllToolStripMenuItem1
            // 
            this.expandAllToolStripMenuItem1.Name = "expandAllToolStripMenuItem1";
            this.expandAllToolStripMenuItem1.Size = new System.Drawing.Size(136, 22);
            this.expandAllToolStripMenuItem1.Text = "Expand All";
            this.expandAllToolStripMenuItem1.Click += new System.EventHandler(this.expandAllToolStripMenuItem1_Click);
            // 
            // collapseAllToolStripMenuItem1
            // 
            this.collapseAllToolStripMenuItem1.Name = "collapseAllToolStripMenuItem1";
            this.collapseAllToolStripMenuItem1.Size = new System.Drawing.Size(136, 22);
            this.collapseAllToolStripMenuItem1.Text = "Collapse All";
            this.collapseAllToolStripMenuItem1.Click += new System.EventHandler(this.collapseAllToolStripMenuItem1_Click);
            // 
            // itemTreeFileNodeRCCM
            // 
            this.itemTreeFileNodeRCCM.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.renameToolStripMenuItem1,
            this.removeToolStripMenuItem,
            this.toolStripMenuItem7,
            this.compressToolStripMenuItem1});
            this.itemTreeFileNodeRCCM.Name = "itemTreeFileNodeRCCM";
            this.itemTreeFileNodeRCCM.Size = new System.Drawing.Size(153, 98);
            // 
            // renameToolStripMenuItem1
            // 
            this.renameToolStripMenuItem1.Name = "renameToolStripMenuItem1";
            this.renameToolStripMenuItem1.Size = new System.Drawing.Size(152, 22);
            this.renameToolStripMenuItem1.Text = "Rename";
            this.renameToolStripMenuItem1.Click += new System.EventHandler(this.renameToolStripMenuItem1_Click);
            // 
            // removeToolStripMenuItem
            // 
            this.removeToolStripMenuItem.Name = "removeToolStripMenuItem";
            this.removeToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.removeToolStripMenuItem.Text = "Remove";
            this.removeToolStripMenuItem.Click += new System.EventHandler(this.removeToolStripMenuItem_Click);
            // 
            // nodeInfoBox
            // 
            this.nodeInfoBox.Controls.Add(this.fileData);
            this.nodeInfoBox.Controls.Add(this.nodeExt);
            this.nodeInfoBox.Controls.Add(this.nodeComp);
            this.nodeInfoBox.Controls.Add(this.nodeSize);
            this.nodeInfoBox.Location = new System.Drawing.Point(316, 53);
            this.nodeInfoBox.Name = "nodeInfoBox";
            this.nodeInfoBox.Size = new System.Drawing.Size(290, 500);
            this.nodeInfoBox.TabIndex = 3;
            this.nodeInfoBox.TabStop = false;
            // 
            // fileData
            // 
            this.fileData.AutoSize = true;
            this.fileData.Location = new System.Drawing.Point(9, 175);
            this.fileData.Name = "fileData";
            this.fileData.Size = new System.Drawing.Size(0, 13);
            this.fileData.TabIndex = 3;
            // 
            // nodeExt
            // 
            this.nodeExt.AutoSize = true;
            this.nodeExt.Location = new System.Drawing.Point(6, 88);
            this.nodeExt.Name = "nodeExt";
            this.nodeExt.Size = new System.Drawing.Size(0, 13);
            this.nodeExt.TabIndex = 2;
            // 
            // nodeComp
            // 
            this.nodeComp.AutoSize = true;
            this.nodeComp.Location = new System.Drawing.Point(6, 66);
            this.nodeComp.Name = "nodeComp";
            this.nodeComp.Size = new System.Drawing.Size(0, 13);
            this.nodeComp.TabIndex = 1;
            // 
            // nodeSize
            // 
            this.nodeSize.AutoSize = true;
            this.nodeSize.Location = new System.Drawing.Point(6, 42);
            this.nodeSize.Name = "nodeSize";
            this.nodeSize.Size = new System.Drawing.Size(0, 13);
            this.nodeSize.TabIndex = 0;
            // 
            // toolStrip1
            // 
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newPackButton,
            this.saveButton,
            this.loadButton,
            this.toolStripSeparator1,
            this.checkBoxT,
            this.delChecked,
            this.toolStripButton1});
            this.toolStrip1.Location = new System.Drawing.Point(0, 24);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(613, 25);
            this.toolStrip1.TabIndex = 4;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // newPackButton
            // 
            this.newPackButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.newPackButton.Image = global::Arfer.Properties.Resources.new_file_icon;
            this.newPackButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.newPackButton.Name = "newPackButton";
            this.newPackButton.Size = new System.Drawing.Size(23, 22);
            this.newPackButton.Text = "Create new package";
            this.newPackButton.Click += new System.EventHandler(this.newPackButton_Click);
            // 
            // saveButton
            // 
            this.saveButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.saveButton.Enabled = false;
            this.saveButton.Image = global::Arfer.Properties.Resources.Actions_document_save_as_icon;
            this.saveButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.saveButton.Name = "saveButton";
            this.saveButton.Size = new System.Drawing.Size(23, 22);
            this.saveButton.Text = "toolStripButton1";
            this.saveButton.ToolTipText = "Save changes";
            this.saveButton.Click += new System.EventHandler(this.saveButton_Click);
            // 
            // loadButton
            // 
            this.loadButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.loadButton.Image = global::Arfer.Properties.Resources.open_file_icon;
            this.loadButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.loadButton.Name = "loadButton";
            this.loadButton.Size = new System.Drawing.Size(23, 22);
            this.loadButton.Text = "Load package";
            this.loadButton.Click += new System.EventHandler(this.loadButton_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 25);
            // 
            // checkBoxT
            // 
            this.checkBoxT.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.checkBoxT.Image = global::Arfer.Properties.Resources.Logos_Register_Editor_icon;
            this.checkBoxT.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.checkBoxT.Name = "checkBoxT";
            this.checkBoxT.Size = new System.Drawing.Size(23, 22);
            this.checkBoxT.Text = "toolStripButton1";
            this.checkBoxT.ToolTipText = "Toggle check boxes";
            this.checkBoxT.Click += new System.EventHandler(this.checkBoxT_Click);
            // 
            // delChecked
            // 
            this.delChecked.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.delChecked.Enabled = false;
            this.delChecked.Image = global::Arfer.Properties.Resources.delete_icon;
            this.delChecked.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.delChecked.Name = "delChecked";
            this.delChecked.Size = new System.Drawing.Size(23, 22);
            this.delChecked.Text = "toolStripButton2";
            this.delChecked.ToolTipText = "Delete selected";
            this.delChecked.Click += new System.EventHandler(this.delChecked_Click);
            // 
            // toolStripButton1
            // 
            this.toolStripButton1.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton1.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton1.Image")));
            this.toolStripButton1.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton1.Name = "toolStripButton1";
            this.toolStripButton1.Size = new System.Drawing.Size(23, 22);
            this.toolStripButton1.Text = "toolStripButton1";
            this.toolStripButton1.Click += new System.EventHandler(this.toolStripButton1_Click);
            // 
            // toolsToolStripMenuItem
            // 
            this.toolsToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.compressToolStripMenuItem,
            this.uncompressToolStripMenuItem,
            this.selectiveCompressionToolStripMenuItem});
            this.toolsToolStripMenuItem.Name = "toolsToolStripMenuItem";
            this.toolsToolStripMenuItem.Size = new System.Drawing.Size(47, 24);
            this.toolsToolStripMenuItem.Text = "Tools";
            // 
            // compressToolStripMenuItem
            // 
            this.compressToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.allToolStripMenuItem,
            this.selectedToolStripMenuItem});
            this.compressToolStripMenuItem.Name = "compressToolStripMenuItem";
            this.compressToolStripMenuItem.Size = new System.Drawing.Size(193, 22);
            this.compressToolStripMenuItem.Text = "Compress...";
            // 
            // allToolStripMenuItem
            // 
            this.allToolStripMenuItem.Name = "allToolStripMenuItem";
            this.allToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.allToolStripMenuItem.Text = "All";
            // 
            // selectedToolStripMenuItem
            // 
            this.selectedToolStripMenuItem.Name = "selectedToolStripMenuItem";
            this.selectedToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.selectedToolStripMenuItem.Text = "Selected";
            // 
            // uncompressToolStripMenuItem
            // 
            this.uncompressToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.allToolStripMenuItem1,
            this.selectedToolStripMenuItem1});
            this.uncompressToolStripMenuItem.Name = "uncompressToolStripMenuItem";
            this.uncompressToolStripMenuItem.Size = new System.Drawing.Size(193, 22);
            this.uncompressToolStripMenuItem.Text = "Uncompress...";
            // 
            // allToolStripMenuItem1
            // 
            this.allToolStripMenuItem1.Name = "allToolStripMenuItem1";
            this.allToolStripMenuItem1.Size = new System.Drawing.Size(152, 22);
            this.allToolStripMenuItem1.Text = "All";
            // 
            // selectedToolStripMenuItem1
            // 
            this.selectedToolStripMenuItem1.Name = "selectedToolStripMenuItem1";
            this.selectedToolStripMenuItem1.Size = new System.Drawing.Size(152, 22);
            this.selectedToolStripMenuItem1.Text = "Selected";
            // 
            // selectiveCompressionToolStripMenuItem
            // 
            this.selectiveCompressionToolStripMenuItem.Name = "selectiveCompressionToolStripMenuItem";
            this.selectiveCompressionToolStripMenuItem.Size = new System.Drawing.Size(193, 22);
            this.selectiveCompressionToolStripMenuItem.Text = "Selective Compression";
            // 
            // toolStripMenuItem7
            // 
            this.toolStripMenuItem7.Name = "toolStripMenuItem7";
            this.toolStripMenuItem7.Size = new System.Drawing.Size(149, 6);
            // 
            // compressToolStripMenuItem1
            // 
            this.compressToolStripMenuItem1.Name = "compressToolStripMenuItem1";
            this.compressToolStripMenuItem1.Size = new System.Drawing.Size(152, 22);
            this.compressToolStripMenuItem1.Text = "Compress";
            this.compressToolStripMenuItem1.Click += new System.EventHandler(this.compressToolStripMenuItem1_Click);
            // 
            // Arfer
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.Control;
            this.ClientSize = new System.Drawing.Size(613, 560);
            this.Controls.Add(this.toolStrip1);
            this.Controls.Add(this.nodeInfoBox);
            this.Controls.Add(this.itemTree);
            this.Controls.Add(this.menuStrip1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.menuStrip1;
            this.MaximizeBox = false;
            this.Name = "Arfer";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Arfer";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Arfer_FormClosing);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.itemTreeNodeRCCM.ResumeLayout(false);
            this.itemTreeFileNodeRCCM.ResumeLayout(false);
            this.nodeInfoBox.ResumeLayout(false);
            this.nodeInfoBox.PerformLayout();
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.TreeView itemTree;
        private System.Windows.Forms.ContextMenuStrip itemTreeRCCM;
        private System.Windows.Forms.ToolStripMenuItem addFolderToolStripMenuItem;
        private System.Windows.Forms.ContextMenuStrip itemTreeNodeRCCM;
        private System.Windows.Forms.ToolStripMenuItem renameToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem addFileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem existingPackageToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem newPackageToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveAsToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem2;
        private System.Windows.Forms.ToolStripMenuItem addFolderToolStripMenuItem1;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem3;
        private System.Windows.Forms.ContextMenuStrip itemTreeFileNodeRCCM;
        private System.Windows.Forms.ToolStripMenuItem renameToolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem removeToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem removeToolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem treeToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem expandAllToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem collapseAllToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem4;
        private System.Windows.Forms.ToolStripMenuItem expandAllToolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem collapseAllToolStripMenuItem1;
        private System.Windows.Forms.GroupBox nodeInfoBox;
        private System.Windows.Forms.Label nodeSize;
        private System.Windows.Forms.Label nodeExt;
        private System.Windows.Forms.Label nodeComp;
        private System.Windows.Forms.Label fileData;
        private System.Windows.Forms.ToolStripMenuItem toggleCheckboxesToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem5;
        private System.Windows.Forms.ToolStripMenuItem deleteSelectedToolStripMenuItem;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripButton checkBoxT;
        private System.Windows.Forms.ToolStripButton delChecked;
        private System.Windows.Forms.ToolStripButton saveButton;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripButton newPackButton;
        private System.Windows.Forms.ToolStripButton loadButton;
        private System.Windows.Forms.ToolStripMenuItem importFromToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem zipToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exportToToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem zipToolStripMenuItem1;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem6;
        private System.Windows.Forms.ToolStripButton toolStripButton1;
        private System.Windows.Forms.ToolStripMenuItem toolsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem compressToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem allToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem selectedToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem uncompressToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem allToolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem selectedToolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem selectiveCompressionToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem7;
        private System.Windows.Forms.ToolStripMenuItem compressToolStripMenuItem1;
    }
}

