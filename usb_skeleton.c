/*
 * USB Skeleton driver - 2.0
 *
 * Copyright (C) 2001-2004 Greg Kroah-Hartman (greg@kroah.com)
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License as
 *	published by the Free Software Foundation, version 2.
 *
 * This driver is based on the 2.6.3 version of drivers/usb/usb-skeleton.c 
 * but has been rewritten to be easy to read and use, as no locks are now
 * needed anymore.
 *
 */


// Below are needed for backwards compatibility to version 2.6.2 of the Linux kernel

#define dbg(fmt, ...) printk(KERN_DEBUG fmt, __VA_ARGS__);

#define dbg1(msg) printk(KERN_DEBUG msg);

#define err(fmt, ...) printk(KERN_ERR fmt, __VA_ARGS__);

#define usb_buffer_alloc usb_alloc_coherent
#define usb_buffer_free usb_free_coherent


/* Remove the below for new versions of kernel */
/* #include <linux/config.h> */
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kref.h>

/* No longer used 
#include <linux/smp_lock.h>
*/

#include <linux/usb.h>
#include <asm/uaccess.h>


/* Define these values to match your devices */
// KFB - Change this for Logitech mouse
#define USB_SKEL_VENDOR_ID	0x046d
#define USB_SKEL_PRODUCT_ID	0xc077

/* table of devices that work with this driver */
static struct usb_device_id skel_table [] = {
	{ USB_DEVICE(USB_SKEL_VENDOR_ID, USB_SKEL_PRODUCT_ID) },
	{ }					/* Terminating entry */
};
MODULE_DEVICE_TABLE (usb, skel_table);


/* Get a minor range for your devices from the usb maintainer */
#define USB_SKEL_MINOR_BASE	192

/* Structure to hold all of our device specific stuff */
struct usb_skel {
	struct usb_device *	udev;			/* the usb device for this device */
	struct usb_interface *	interface;		/* the interface for this device */
	unsigned char *		bulk_in_buffer;		/* the buffer to receive data */
	size_t			bulk_in_size;		/* the size of the receive buffer */
	__u8			bulk_in_endpointAddr;	/* the address of the bulk in endpoint */
	__u8			bulk_out_endpointAddr;	/* the address of the bulk out endpoint */
	
	unsigned char * 	int_in_buffer;		// Buffer to receive USB interrupt frames
	size_t			int_in_size;
	__u8			int_in_endpointAddr;
	
	struct kref		kref;
	// Added this
	struct urb 		*my_urb;
	
};
#define to_skel_dev(d) container_of(d, struct usb_skel, kref)

static struct usb_driver skel_driver;

static void skel_delete(struct kref *kref)
{	
	struct usb_skel *dev = to_skel_dev(kref);

	usb_put_dev(dev->udev);
	kfree (dev->bulk_in_buffer);
	kfree (dev);
}

static int skel_open(struct inode *inode, struct file *file)
{
	struct usb_skel *dev;
	struct usb_interface *interface;
	int subminor;
	int retval = 0;

	subminor = iminor(inode);

	interface = usb_find_interface(&skel_driver, subminor);
	if (!interface) {
		err ("%s - error, can't find device for minor %d",
		     __FUNCTION__, subminor);
		retval = -ENODEV;
		goto exit;
	}

	dev = usb_get_intfdata(interface);
	if (!dev) {
		retval = -ENODEV;
		goto exit;
	}
	
	/* increment our usage count for the device */
	kref_get(&dev->kref);

	/* save our object in the file's private structure */
	file->private_data = dev;

exit:
	return retval;
}

static int skel_release(struct inode *inode, struct file *file)
{
	struct usb_skel *dev;

	dev = (struct usb_skel *)file->private_data;
	if (dev == NULL)
		return -ENODEV;

	/* decrement the count on our device */
	kref_put(&dev->kref, skel_delete);
	return 0;
}

static ssize_t skel_read(struct file *file, char __user *buffer, size_t count, loff_t *ppos)
{
	struct usb_skel *dev;
	int retval = 0;
        // KFB - type has changed to int * 
        int my_count = count;

	dev = (struct usb_skel *)file->private_data;
	
	/* do a blocking bulk read to get data from the device */
	retval = usb_bulk_msg(dev->udev,
			      usb_rcvbulkpipe(dev->udev, dev->bulk_in_endpointAddr),
			      dev->bulk_in_buffer,
			      min(dev->bulk_in_size, count),
			      &my_count, HZ*10);

	/* if the read was successful, copy the data to userspace */
	if (!retval) {
		if (copy_to_user(buffer, dev->bulk_in_buffer, count))
			retval = -EFAULT;
		else
			retval = count;
	}

	return retval;
}


// KFB - The new kernel callback function doesn't include pt_regs.  This function doesn't use those, so goody!
static void skel_write_bulk_callback(struct urb *urb)
{
	/* sync/async unlink faults aren't errors */
	if (urb->status && 
	    !(urb->status == -ENOENT || 
	      urb->status == -ECONNRESET ||
	      urb->status == -ESHUTDOWN)) {
		dbg("%s - nonzero write bulk status received: %d",
		    __FUNCTION__, urb->status);
	}

	/* free up our allocated buffer */
	usb_buffer_free(urb->dev, urb->transfer_buffer_length, 
			urb->transfer_buffer, urb->transfer_dma);
}

static ssize_t skel_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *ppos)
{
	struct usb_skel *dev;
	int retval = 0;
	struct urb *urb = NULL;
	char *buf = NULL;

	dev = (struct usb_skel *)file->private_data;

	/* verify that we actually have some data to write */
	if (count == 0)
		goto exit;

	/* create a urb, and a buffer for it, and copy the data to the urb */
	urb = usb_alloc_urb(0, GFP_KERNEL);
	if (!urb) {
		retval = -ENOMEM;
		goto error;
	}

	buf = usb_buffer_alloc(dev->udev, count, GFP_KERNEL, &urb->transfer_dma);
	if (!buf) {
		retval = -ENOMEM;
		goto error;
	}
	if (copy_from_user(buf, user_buffer, count)) {
		retval = -EFAULT;
		goto error;
	}

	/* initialize the urb properly */
	/* typedef void (*usb_complete_t)(struct urb *);  */

	usb_fill_bulk_urb(urb, dev->udev,
			  usb_sndbulkpipe(dev->udev, dev->bulk_out_endpointAddr),
			  buf, count, skel_write_bulk_callback, dev);
	urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

	/* send the data out the bulk port */
	retval = usb_submit_urb(urb, GFP_KERNEL);
	if (retval) {
		printk(KERN_ERR "%s - failed submitting write urb, error %d", __FUNCTION__, retval);
		goto error;
	}

	/* release our reference to this urb, the USB core will eventually free it entirely */
	usb_free_urb(urb);

exit:
	return count;

error:
	usb_buffer_free(dev->udev, count, buf, urb->transfer_dma);
	usb_free_urb(urb);
	kfree(buf);
	return retval;
}

static struct file_operations skel_fops = {
	// KFB - commented out   .owner =	THIS_MODULE,
	.read =		skel_read,
	.write =	skel_write,
	.open =		skel_open,
	.release =	skel_release,
};

/* 
 * usb class driver info in order to get a minor number from the usb core,
 * and to have the device registered with devfs and the driver core
 */
static struct usb_class_driver skel_class = {
	.name = "usb/skel%d",
	.fops = &skel_fops,
	// KFB - commented out below
        //.mode = S_IFCHR | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH,
	.minor_base = USB_SKEL_MINOR_BASE,
};


static void urb_complete_callback(struct urb *my_urb)
{	int retval;
        unsigned char *my_point = my_urb->transfer_buffer;
	int horizontal, vertical;
	char direction[20] = "";
	
	horizontal = *(my_point + 1);
	vertical = *(my_point + 2);
	
	if (vertical > 128)
	  strcat(direction, "UP ");
	else if (vertical > 0)
	  strcat(direction, "DOWN ");
	
	if (horizontal > 128)
	  strcat(direction, "LEFT ");
	else if (horizontal > 0)
	  strcat(direction, "RIGHT ");
  
	
	printk(KERN_INFO "MOUSE moved %s....%d bytes in. Vertical=%d, Horizontal=%d \n", direction, my_urb->actual_length, vertical, horizontal);
	
	retval = usb_submit_urb(my_urb, GFP_KERNEL);
	if (retval) 
		printk(KERN_ERR "%s - failed submitting write urb, error %d", __FUNCTION__, retval);
}



static int skel_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
	struct usb_skel *dev = NULL;
	struct usb_device *my_usb_dev;
	
	struct usb_host_interface *iface_desc;
	struct usb_endpoint_descriptor *endpoint;
	size_t buffer_size;
	int i;
	int retval = -ENOMEM;
	
	/* allocate memory for our device state and initialize it */
	dev = kmalloc(sizeof(struct usb_skel), GFP_KERNEL);
	if (dev == NULL) {
		printk(KERN_ERR "Out of memory");
		goto error;
	}
	memset(dev, 0x00, sizeof (*dev));
	kref_init(&dev->kref);

	dev->udev = usb_get_dev(interface_to_usbdev(interface));
	dev->interface = interface;

	my_usb_dev = dev->udev;
	// Print out information about the device
	printk(KERN_INFO "USB-SKEL: Probe activated \n");
	printk(KERN_INFO "Device Number = %d\n", my_usb_dev->devnum);
	printk(KERN_INFO "Device Path=%s\n", my_usb_dev->devpath);
	printk(KERN_INFO "Bus mA=%d", my_usb_dev->bus_mA);
        printk(KERN_INFO "Speed is %d\n", my_usb_dev->speed);
	printk(KERN_INFO "Can submit URBs %d\n", my_usb_dev->can_submit);
	if (my_usb_dev->product != NULL)
	   printk(KERN_INFO "Product String=%s\n", my_usb_dev->product);
	else
	  printk(KERN_INFO "Product String=NULL\n");
	
	if (my_usb_dev->manufacturer != NULL)
	  printk(KERN_INFO "Manufacturer=%s\n", my_usb_dev->manufacturer);
	else
	  printk(KERN_INFO "Manufacturer=NULL\n");
	
	if (my_usb_dev->serial != NULL)
	  printk(KERN_INFO "Serial=%s\n", my_usb_dev->serial);
	else
	  printk(KERN_INFO "Serial=NULL\n");

	/* set up the endpoint information */
	/* use only the first bulk-in and bulk-out endpoints */
	iface_desc = interface->cur_altsetting;
	for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i) {
		int the_end;

		endpoint = &iface_desc->endpoint[i].desc;
 		printk(KERN_INFO "Found endpoint %d, type is %d\n", i, endpoint->bmAttributes);
        	printk(KERN_INFO "Packetsize is %d\n", endpoint->wMaxPacketSize);
		printk(KERN_INFO "Endpoint address is %d\n", endpoint->bEndpointAddress);
	
		the_end = endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK;
		if (the_end == USB_ENDPOINT_XFER_INT)
		{
			printk(KERN_INFO  "This is my interrupt endpoint, setting up URB to receive information\n");
			buffer_size = endpoint->wMaxPacketSize;
			dev->int_in_size = buffer_size; // wMaxPacketSize comes in as 4 for some reason - extend to 128
			dev->int_in_endpointAddr = endpoint->bEndpointAddress;
			dev->int_in_buffer = kmalloc(buffer_size, GFP_KERNEL);
			if (!dev->int_in_buffer) {
				printk(KERN_ERR "Could not allocate int_in_buffer");
				goto error;
			}
			printk(KERN_INFO  "Allocated buffer of size %ld address %p\n", dev->int_in_size, dev->int_in_buffer);
			
			dev->my_urb = usb_alloc_urb(0, GFP_KERNEL);
			
			usb_fill_int_urb(dev->my_urb, 
			    dev->udev,
			    usb_rcvintpipe(dev->udev, endpoint->bEndpointAddress),
			    dev->int_in_buffer,
			    dev->int_in_size,
			    urb_complete_callback,
			    NULL,
			    endpoint->bInterval);

		retval = usb_submit_urb(dev->my_urb, GFP_KERNEL);
		printk(KERN_INFO "Return value from usb_submit was %d \n", retval);
		
		}
    
		/*
		if (!dev->bulk_in_endpointAddr &&
		    (endpoint->bEndpointAddress & USB_DIR_IN) &&
		    ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK)
					== USB_ENDPOINT_XFER_BULK)) {
			buffer_size = endpoint->wMaxPacketSize;
			dev->bulk_in_size = buffer_size;
			dev->bulk_in_endpointAddr = endpoint->bEndpointAddress;
			dev->bulk_in_buffer = kmalloc(buffer_size, GFP_KERNEL);
			if (!dev->bulk_in_buffer) {
				printk(KERN_ERR "Could not allocate bulk_in_buffer");
				goto error;
			}
		}

		if (!dev->bulk_out_endpointAddr &&
		    !(endpoint->bEndpointAddress & USB_DIR_IN) &&
		    ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK)
					== USB_ENDPOINT_XFER_BULK)) {
			dev->bulk_out_endpointAddr = endpoint->bEndpointAddress;
		}
		*/

	}

        /*
	if (!(dev->bulk_in_endpointAddr)) {
		printk(KERN_ERR "Could not find bulk-in endpoint");
		goto error;
	}

	if (!(dev->bulk_out_endpointAddr)) {
		printk(KERN_ERR "Could not find bulk-out endpoint");
		goto error;
	}
        */


	/* save our data pointer in this interface device */
	usb_set_intfdata(interface, dev);

	/* we can register the device now, as it is ready */
	retval = usb_register_dev(interface, &skel_class);
	if (retval) {
		/* something prevented us from registering this driver */
		printk(KERN_ERR  "Not able to get a minor for this device.");
		usb_set_intfdata(interface, NULL);
		goto error;
	}

	/* let the user know what node this device is now attached to */
	printk(KERN_INFO "USB Skeleton device now attached to USBSkel-%d", interface->minor);
	return 0;

error:
	if (dev)
		kref_put(&dev->kref, skel_delete);
	return retval;
}




static void skel_disconnect(struct usb_interface *interface)
{
	struct usb_skel *dev;
	int minor = interface->minor;

	/* prevent skel_open() from racing skel_disconnect() */
	
        /* KFB - need to replace with per driver mutex
        lock_kernel();
        http://stackoverflow.com/questions/5956145/can-someone-help-me-replace-lock-kernel-on-a-block-device-driver
        */

	dev = usb_get_intfdata(interface);
	usb_set_intfdata(interface, NULL);

	/* give back our minor */
	usb_deregister_dev(interface, &skel_class);

        /* 	unlock_kernel();   SEE ABOVE */

	/* decrement our usage count */
	kref_put(&dev->kref, skel_delete);

	printk(KERN_INFO  "USB Skeleton #%d now disconnected", minor);
}

static struct usb_driver skel_driver = {
	//  commented out KFB .owner = THIS_MODULE,
	.name = "skeleton",
	.id_table = skel_table,
	.probe = skel_probe,
	.disconnect = skel_disconnect,
};

static int __init usb_skel_init(void)
{
	int result;

        dbg1("SKELETON: Initializing function \n");

	/* register this driver with the USB subsystem */
	result = usb_register(&skel_driver);
	if (result)
		printk(KERN_ERR  "usb_register failed. Error number %d", result);

	return result;
}

static void __exit usb_skel_exit(void)
{
	/* deregister this driver with the USB subsystem */
	usb_deregister(&skel_driver);
}

module_init (usb_skel_init);
module_exit (usb_skel_exit);

MODULE_LICENSE("GPL");
