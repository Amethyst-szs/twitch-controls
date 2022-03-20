// Require the necessary discord.js classes
const { Client, Intents } = require('discord.js');
const { SlashCommandBuilder } = require('@discordjs/builders');
const { REST } = require('@discordjs/rest');
const { Routes } = require('discord-api-types/v9');
const { disclientid, disguildid, distoken } = require('../settings/secret.json');

//Require other modules
const log = require('../server_bin/console');

// Create a new client instance
const client = new Client({ intents: [Intents.FLAGS.GUILDS] });

//Global variables
let costFact = undefined;
let cooldownMulti = 1;

function writeStatus(){
	client.user.setActivity(`TC - Cost Factor: ${costFact} Cooldown Factor: ${cooldownMulti}`);
}

module.exports = {
	botManage: function(){
		// When the client is ready, run this code (only once)
		client.once('ready', () => {
			log.log(3, "Initalized Discord Bot for managing the server!");
			client.user.setActivity(`Twitch Controls - Default Settings`);
		});

		client.on('interactionCreate', async interaction => {
			if (!interaction.isCommand()) return;
			interaction.options.getString()
			const { commandName } = interaction;
			const main = require('../main');
			
			switch(commandName){
				case "ping":
					log.log(3, "Pong!");
					await interaction.reply('Pong!');
					break;
				case "cost":
					main.disCostUpdate(interaction.options.getString('modification', true), interaction.options.getNumber('value', true));
					log.log(3, `Pushing a cost update from ${interaction.user.username}`);
					interaction.reply(`Performing **${interaction.options.getString('modification', true)}** operation on cost!!`);
					costFact = interaction.options.getNumber('value', false);
					writeStatus();
					break;
				case "cooldown":
					value = interaction.options.getNumber('value', true);

					main.disCooldownUpdate(value);
					log.log(3, `Pushing a cooldown update from ${interaction.user.username}`);
					interaction.reply(`Setting the cooldown multiplier to **${value}**!`);
					cooldownMulti = value;
					writeStatus();
					break;
				default:
					interaction.reply("**ERROR**\nDunno what you did, but it didn't work");
					log.log(3, "Strange interaction, ignoring");
					break;
			}
		});

		// Login to Discord with your client's token
		client.login(distoken);
	},
	
	slashCommandInit: function(){
		const commands = [
			new SlashCommandBuilder()
				.setName('ping')
				.setDescription('Verifies the server is still running by sending back a pong'),
			new SlashCommandBuilder()
				.setName('cost')
				.setDescription('Update the channel point pricing on the fly')
				.addStringOption(option =>
					option.setName('modification')
					.setDescription('How do you want to change your pricing')
					.setRequired(true)
					.addChoice('Increase', 'inc')
					.addChoice('Decrease', 'dec')
					.addChoice('Set', 'set')
					.addChoice('Toggle Debug Pricing', 'dis')
				)
				.addNumberOption(option =>
					option.setName('value')
						.setDescription('How much to increase or decrease price multiplier OR set it to')
						.setRequired(true)
				),
			new SlashCommandBuilder()
				.setName('cooldown')
				.setDescription('Set a multiplier on redeem cooldowns!')
				.addNumberOption(option =>
					option.setName('value')
						.setDescription('What should the multiplier for cooldowns be?')
						.setRequired(true)
				)
		]
			.map(command => command.toJSON());
		
		const rest = new REST({ version: '9' }).setToken(distoken);
		
		rest.put(Routes.applicationGuildCommands(disclientid, disguildid), { body: commands })
			.then(() => log.log(3, 'Successfully registered slash commands'))
			.catch(console.error);
	}
}