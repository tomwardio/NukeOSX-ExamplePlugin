
#include "DDImage/Iop.h"
#include "DDImage/Row.h"
#include "DDImage/Knobs.h"
#include "DDImage/DrawIop.h"

using namespace DD::Image;

class ExamplePlugin : public Iop
{
public:
    
    ExamplePlugin(Node* node) : Iop(node)
    {}
    
    virtual ~ExamplePlugin()
    {}
    
    // Set the min and max inputs, you can change this as your requirements
    int minimum_inputs() const { return 1; }
    int maximum_inputs() const { return 1; }
    
    virtual void knobs(Knob_Callback);

    // The first step in Nuke is to validate
    void _validate(bool);
    void _request(int x, int y, int r, int t, ChannelMask channels, int count);
    
    //! This function does all the work.
    void engine ( int y, int x, int r, ChannelMask channels, Row& outRow );
    
    //! Return the name of the class.
    const char* Class() const { return CLASS; }
    const char* node_help() const { return HELP; }
    
private:
    
    //! Information to the plug-in manager of DDNewImage/NUKE.
    static const Iop::Description   DESCRIPTION;
    static const char*              CLASS;
    static const char*              HELP;
};


void ExamplePlugin::knobs(Knob_Callback f)
{
    // Add knob logic here
}

/*! This is a function that creates an instance of the operator, and is
 needed for the Iop::Description to work.
 */
static Iop* ExamplePluginCreate(Node* node)
{
    return new ExamplePlugin(node);
}

/*! The Iop::Description is how NUKE knows what the name of the operator is,
 how to create one, and the menu item to show the user. The menu item may be
 0 if you do not want the operator to be visible.
 */
const Iop::Description ExamplePlugin::DESCRIPTION ( CLASS, "Examples/ExamplePlugin",
                                                   ExamplePluginCreate );


// No matter what the name of the class is , the below is the actual name of the plugin inside Nuke.
const char* ExamplePlugin::CLASS = "ExamplePlugin";
const char* ExamplePlugin::HELP = "Write the description of your plugin here.\n";

/*Essentially, the _validate function tells NUKE about the size and format of the generated images,
	including the image channels we’re going to access and create. In this example, copy_info()
	is called, which takes the format information from the input image and copies it onto the format
	of the current “Basic” node.The set_out_channels(Mask_All); call simply says that we are looking
	at (and modifying) all channels in the image. The Mask_All could be replaced by Mask_Red or Mask_Alpha
	if we just wanted to look at the red or alpha image channels respectively. */

void ExamplePlugin::_validate(bool for_real)
{
    copy_info(0); // copy bbox channels etc from input0, which will validate it.

    info_.channels(Mask_RGB);
}


/*The in_channels function is required by the parent PixelIop class and tells NUKE
 what channels of the image we would like to access. In general, we would like all channels,
	and so leave ChannelSet& mask untouched. If we wanted, we could restrict the
	channels being operated on, for example, if we only desired the red channel: mask &= Mask_Red;. */

void ExamplePlugin::_request(int x, int y, int r, int t, ChannelMask channels, int count)
{
    // for this example, we're only interested in the RGB channels
    input(0)->request( x, y, r, t, ChannelMask(Mask_RGB), count );
}


/*! For each line in the area passed to request(), this will be called. It must
 calculate the image data for a region at vertical position y, and between
 horizontal positions x and r, and write it to the passed row
 structure. Usually this works by asking the input for data, and modifying
 it.
 
 */

/*Notice that the class Row is used a lot in this function. This is because NUKE likes
	to operate on scanline rows of images at a time. This makes sense for many reasons,
	such as operating on different scanlines in parallel (multiple threads) or, more importantly,
	only needing to keep a handful of image rows in memory at any given time. The row index you
	are currently working on is given by y, with the offset into the scanline given by x.
	Note that either or both of these can be negative (for example, if the input image is
	outside the bounds of the frame rectangle). The last index of the scanline is given by r,
	and the width of the input scanline is therefore given by r-x-1. The channels that you have
	access to are set in the channels. As you can see from the code, in[z]+x and out.writable(z)+x
	give you pointers to the input and output row data for channel z, where z is obtained using the
	foreach function to iterate over the collection of channels passed in. The output pointer can
	obviously then be accessed to do what ever you’d like. */



void ExamplePlugin::engine ( int y, int x, int r,
                            ChannelMask channels, Row& outRow )
{
    // Remember that this function is being called for every row and is being
    // parallel executed by the threads
    // for this example, create a simple greyscale output from RGB channels
    ChannelMask rgbMask(Mask_RGB);
    
    Row inputRow(x, r);
    inputRow.get(input0(), y, x, r, rgbMask);
    
    for( int curX = x ; curX < r; curX++ ) {
        
        float newValue = 0.0f;
        
        foreach ( channel, rgbMask ) {
            newValue += inputRow[channel][curX];
        }
        newValue /= channels.size();
        
        foreach ( channel, channels ) {
            outRow.writable(channel)[curX] = newValue;
        }
    }
}

static Iop* build(Node* node) { return new ExamplePlugin(node); }