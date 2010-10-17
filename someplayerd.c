/*
 * This file is used a play an mp3 file
 * Author : suman.subj@gmail.com
 * gst-launch filesrc location=Darling-Neeve.Mp3 ! mad ! audioconvert ! audioresample ! alsasink
 * gst-launch filesrc location=Darling-Neeve.Mp3 ! mad ! audioconvert ! audioresample ! osssink
 * 
 *
 */

#include<stdio.h>
#include <gst/gst.h>
#include <glib.h>


static gboolean bus_call (GstBus     *bus,
          GstMessage *msg,
          gpointer    data)
{
	GMainLoop *loop = (GMainLoop *) data;

	switch (GST_MESSAGE_TYPE (msg)) {
	
		case GST_MESSAGE_EOS:{
			g_print ("End of stream\n");
			g_main_loop_quit (loop);
			break;
		}

		case GST_MESSAGE_ERROR: {
			gchar  *debug;
			GError *error;

			gst_message_parse_error (msg, &error, &debug);
			g_free (debug);

			g_printerr ("Error: %s\n", error->message);
			g_error_free (error);

			g_main_loop_quit (loop);
			break;
	    	}	
		default:
			break;
	}

	return TRUE;
}


int main(int argc,char* argv[])
{

	gst_init(&argc,&argv);

	GMainLoop* loop = g_main_loop_new(NULL,FALSE);
	gchar* file_path=argv[1];
	if(!g_file_test(file_path,  G_FILE_TEST_IS_REGULAR))
		g_message("!!! file not present !!!");

	/*creating a piple line*/
	GstElement *somePipeline = gst_pipeline_new("somePlayerPipe");

	
	GstElement* source  = gst_element_factory_make ("filesrc","file-source");	
	g_object_set(source,"location",file_path,NULL);

	GstElement* audio_decoder  = gst_element_factory_make ("mad","audio_decoder");	
	GstElement* audio_converter  = gst_element_factory_make ("audioconvert","converter");	
	GstElement* audio_resample= gst_element_factory_make ("audioresample","resample");	
	GstElement* osssink= gst_element_factory_make ("osssink","osssink");	

	if(!source || !audio_decoder || !audio_converter || !audio_resample || !osssink)	
	{
		g_printerr("one element could not be created");
		return -1;
	}

	GstBus* somePlayerBus = gst_pipeline_get_bus(GST_PIPELINE(somePipeline));
	gst_bus_add_watch(somePlayerBus,bus_call,loop);
	gst_object_unref(somePlayerBus);

	gst_bin_add_many (GST_BIN (somePipeline),source, 
		audio_decoder, audio_converter, audio_resample, osssink, NULL);

        /* we link the elements together */
        /* file-source -> audio-decoder -> converter -> resample -> osssink */
	gst_element_link_many (source, audio_decoder, audio_converter,audio_resample, osssink, NULL);
	/* Set the pipeline to "playing" state*/
	g_print ("Now playing: %s\n", file_path);
	gst_element_set_state(somePipeline,GST_STATE_PLAYING);
	g_main_loop_run(loop);

	/* out of mainloop,*/
	gst_element_set_state(somePipeline,GST_STATE_NULL);
	gst_object_unref(GST_OBJECT(somePipeline));
	return 0;
}
